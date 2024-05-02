#include <assert.h>
#include <erl_nif.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include <jq.h>

struct jq_resource {
  jq_state * jq;
};

ErlNifResourceType * jq_resource_type;

void free_jq_resource(ErlNifEnv * env, void * obj) {
  struct jq_resource * jq_resource = (struct jq_resource *) obj;
  free(jq_resource->jq);
  jq_resource->jq = NULL;
}

int open_jq_resource_type(ErlNifEnv * env) {
  ErlNifResourceFlags tried;
  jq_resource_type = enif_open_resource_type(env, NULL, "jq", free_jq_resource, ERL_NIF_RT_CREATE, &tried);
  return jq_resource_type != NULL;
}

ERL_NIF_TERM make_jq_resource(ErlNifEnv * env, jq_state * jq) {
  struct jq_resource * jq_resource = enif_alloc_resource(jq_resource_type, sizeof(struct jq_resource));
  jq_resource->jq = jq;
  return enif_make_resource(env, jq_resource);
}

int get_jq_resource(ErlNifEnv * env, ERL_NIF_TERM arg, jq_state ** jq) {
  struct jq_resource * jq_resource;

  if (!enif_get_resource(env, arg, jq_resource_type, (void **) &jq_resource)) {
    return 0;
  }
  *jq = jq_resource->jq;
  return 1;
}


ERL_NIF_TERM ok_atom;
ERL_NIF_TERM error_atom;
ERL_NIF_TERM nil_atom;
ERL_NIF_TERM false_atom;
ERL_NIF_TERM true_atom;

void init_atoms(ErlNifEnv * env) {
  ok_atom = enif_make_atom(env, "ok");
  error_atom = enif_make_atom(env, "error");
  nil_atom = enif_make_atom(env, "nil");
  false_atom = enif_make_atom(env, "false");
  true_atom = enif_make_atom(env, "true");
}

int term_to_jv(ErlNifEnv * env, ERL_NIF_TERM term, jv * result) {

  long int integer;
  double number;
  ErlNifBinary string;

  if (term == nil_atom) {

    *result = jv_null();
    return 1;
  }

  if (term == false_atom) {
    *result = jv_false();
    return 1;
  }

  if (term == true_atom) {
    *result = jv_true();
    return 1;
  }

  if (enif_is_atom(env, term)) {
    unsigned int size;
    assert(enif_get_atom_length(env, term, &size, ERL_NIF_UTF8));
    char * data = malloc(size + 1);
    assert(enif_get_atom(env, term, data, size + 1, ERL_NIF_UTF8));
    *result = jv_string(data);
    free(data);
    return 1;
  }

  if (enif_get_int64(env, term, &integer)) {
    *result = jv_number(integer);
    return 1;
  }

  if (enif_get_double(env, term, &number)) {
    *result = jv_number(number);
    return 1;
  }

  if (enif_inspect_binary(env, term, &string)) {
    char * data = malloc(string.size + 1);
    memcpy(data, string.data, string.size);
    data[string.size] = 0;
    *result = jv_string(data);
    free(data);
    return 1;
  }

  if (enif_is_list(env, term)) {
    unsigned int length;
    if (!enif_get_list_length(env, term, &length)) {
      return 0;
    }
    *result = jv_array_sized(length);
    ERL_NIF_TERM head, tail = term;
    for (int i = 0; enif_get_list_cell(env, tail, &head, &tail); i++) {
      jv element;
      if (!term_to_jv(env, head, &element)) {
        return 0;
      }
      *result = jv_array_set(*result, i, element);
    }
    return 1;
  }

  if (enif_is_map(env, term)) {
    *result = jv_object();
    ErlNifMapIterator iter;
    assert(enif_map_iterator_create(env, term, &iter, ERL_NIF_MAP_ITERATOR_FIRST));
    ERL_NIF_TERM key_term, value_term;
    jv key_jv, value_jv;
    while (enif_map_iterator_get_pair(env, &iter, &key_term, &value_term)) {
      if (!term_to_jv(env, key_term, &key_jv) ||
          !term_to_jv(env, value_term, &value_jv)) {
        enif_map_iterator_destroy(env, &iter);
        return 0;
      }
      *result = jv_object_set(*result, key_jv, value_jv);
      enif_map_iterator_next(env, &iter);
    }
    enif_map_iterator_destroy(env, &iter);
    return 1;
  }

  return 0;
}

int jv_to_term(ErlNifEnv * env, jv value, ERL_NIF_TERM * term) {
  int size;
  int length;

  switch (jv_get_kind(jv_copy(value))) {
  case JV_KIND_NULL:
    *term = nil_atom;
    return 1;

  case JV_KIND_FALSE:
    *term = false_atom;
    return 1;

  case JV_KIND_TRUE:
    *term = true_atom;
    return 1;

  case JV_KIND_NUMBER:
    if (jv_is_integer(jv_copy(value))) {
      *term = enif_make_int64(env, jv_number_value(value));
    } else {
      *term = enif_make_double(env, jv_number_value(value));
    }
    return 1;

  case JV_KIND_STRING:
    size = jv_string_length_bytes(jv_copy(value));
    unsigned char * data = enif_make_new_binary(env, size, term);
    memcpy(data, jv_string_value(value), size);
    return 1;

  case JV_KIND_ARRAY:
    length = jv_array_length(jv_copy(value));
    *term = enif_make_list(env, 0);
    for (int i = length - 1; i >= 0; i--) {
      jv element_jv = jv_array_get(jv_copy(value), i);
      ERL_NIF_TERM element;
      if (!jv_to_term(env, element_jv, &element)) {
        return 0;
      }
      *term = enif_make_list_cell(env, element, *term);
    }
    return 1;

  case JV_KIND_OBJECT:
    *term = enif_make_new_map(env);
    ERL_NIF_TERM key_term, value_term;

    jv_object_foreach(value, key_jv, value_jv) {
      if (!jv_to_term(env, key_jv, &key_term) ||
          !jv_to_term(env, value_jv, &value_term) ||
          !enif_make_map_put(env, *term, key_term, value_term, term)) {
        return 0;
      }
    };

    return 1;

  case JV_KIND_INVALID:
  default:
    return 0;
  }
}

static ERL_NIF_TERM encode_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  jv value;

  if (argc != 1 ||
      !term_to_jv(env, argv[0], &value)) {
    return enif_make_badarg(env);
  }

  int flags = 0;
  jv result = jv_dump_string(value, flags);

  ERL_NIF_TERM result_term;
  if (!jv_to_term(env, result, &result_term)) {
    return enif_make_badarg(env);
  }
  return result_term;
}

static ERL_NIF_TERM decode_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifBinary string;

  if (argc != 1 ||
      !enif_inspect_binary(env, argv[0], &string)) {
    return enif_make_badarg(env);
  }

  jv result = jv_parse_sized((char *) string.data, string.size);

  ERL_NIF_TERM result_term;
  if (!jv_to_term(env, result, &result_term)) {
    return enif_make_badarg(env);
  }
  return result_term;
}

static ERL_NIF_TERM compile_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifBinary program_string;

  if (argc != 1 ||
      !enif_inspect_binary(env, argv[0], &program_string)) {
    return enif_make_badarg(env);
  }

  char * program = malloc(program_string.size + 1);
  memcpy(program, program_string.data, program_string.size);
  program[program_string.size] = 0;

  jq_state * jq = jq_init();

  if (!jq_compile(jq, program)) {
    free(program);
    return enif_make_badarg(env);
  }

  free(program);
  return make_jq_resource(env, jq);
}

static ERL_NIF_TERM run_nif(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[]) {
  jq_state * jq;
  ErlNifBinary input_string;

  if (argc != 2 ||
      !get_jq_resource(env, argv[0], &jq) ||
      !enif_inspect_binary(env, argv[1], &input_string)) {
    return enif_make_badarg(env);
  }

  jv input = jv_parse_sized((char *) input_string.data, input_string.size);

  if (!jv_is_valid(input)) {
    return enif_make_badarg(env);
  }

  int flags = 0;
  jq_start(jq, input, flags);

  ERL_NIF_TERM result_list = enif_make_list(env, 0);
  jv value;

  while (jv_is_valid(value = jq_next(jq))) {
    ERL_NIF_TERM value_term;
    if (!jv_to_term(env, value, &value_term)) {
      return enif_make_badarg(env);
    }
    result_list = enif_make_list_cell(env, value_term, result_list);
  }

  return result_list;
}

static ErlNifFunc nif_funcs[] = {
  {"encode!", 1, encode_nif},
  {"decode!", 1, decode_nif},
  {"compile", 1, compile_nif},
  {"run", 2, run_nif},
};

int load(ErlNifEnv * env, void ** priv_data, ERL_NIF_TERM load_info) {
  init_atoms(env);
  assert(open_jq_resource_type(env));
  return 0;
}

ERL_NIF_INIT(Elixir.JQ.NIF, nif_funcs, load, NULL, NULL, NULL)
