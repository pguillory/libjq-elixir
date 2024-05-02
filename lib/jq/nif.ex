defmodule JQ.NIF do
  @on_load :init

  @doc false
  def init do
    priv_dir = :code.priv_dir(:libjq)
    nif_path = Path.join(priv_dir, "jq")
    :ok = :erlang.load_nif(nif_path, 0)
  end

  def encode!(_value), do: exit(:nif_not_loaded)
  def decode!(_json), do: exit(:nif_not_loaded)
  def compile(_program), do: exit(:nif_not_loaded)
  def run(_compiled_program, _input), do: exit(:nif_not_loaded)
end
