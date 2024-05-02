defmodule Libjq.MixProject do
  use Mix.Project

  def project do
    [
      package: package(),
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

  defp package do
    [
      description: "Elixir bindings for libjq, the library form of the command-line tool jq",
      licenses: ["MIT"],
      maintainers: ["pguillory@gmail.com"],
      links: %{
        "GitHub" => "https://github.com/pguillory/libjq-elixir"
      }
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
