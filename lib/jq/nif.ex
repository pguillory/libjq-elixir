defmodule JQ.NIF do
  @on_load :init

  @doc false
  def init do
    priv_dir = :code.priv_dir(:jq)
    nif_path = Path.join(priv_dir, "jq")
    :ok = :erlang.load_nif(nif_path, 0)
  end

  def dump_string(_value), do: exit(:nif_not_loaded)
end
