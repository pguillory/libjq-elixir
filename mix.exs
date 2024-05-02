defmodule JQ.MixProject do
  use Mix.Project

  def project do
    [
      app: :libjq,
      version: "0.1.0",
      elixir: "~> 1.16",
      start_permanent: Mix.env() == :prod,
      elixirc_options: elixirc_options(),
      compilers: [:elixir_make] ++ Mix.compilers(),
      make_targets: ["nifs"],
      make_clean: ["clean"],
      deps: deps()
    ]
  end

  def elixirc_options do
    case Mix.env() do
      :test -> [warnings_as_errors: true]
      _ -> []
    end
  end

  def application do
    [
      extra_applications: [:logger]
    ]
  end

  defp deps do
    [
      {:elixir_make, ">= 0.0.0", runtime: false},
      {:ex_doc, ">= 0.0.0", only: :dev, runtime: false}
    ]
  end
end
