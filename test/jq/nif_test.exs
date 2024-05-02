defmodule JQ.NIFTest do
  import JQ.NIF
  use ExUnit.Case

  test "dump_string" do
    assert dump_string(nil) == "null"
    assert dump_string(false) == "false"
    assert dump_string(true) == "true"
    assert dump_string(123) == "123"
    assert dump_string(4.5) == "4.5"
    assert dump_string("abc") == "\"abc\""
    assert dump_string([]) == "[]"
    assert dump_string([1, 2]) == "[1,2]"
    assert dump_string([[1], [2]]) == "[[1],[2]]"
    assert dump_string(%{}) == "{}"
    assert dump_string(%{"a" => 1}) == "{\"a\":1}"
    assert dump_string(%{a: 1}) == "{\"a\":1}"
  end
end
