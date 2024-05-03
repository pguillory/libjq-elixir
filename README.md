# Libjq

Elixir bindings for libjq, library form of the popular command-line JSON
processing tool [jq].

[jq]: https://github.com/jqlang/jq

## Example

```elixir
iex> program = Libjq.compile(".x")
iex> json = ~S[ {"x": 12345} ]
iex> Libjq.run(program, json)
[12345]
```

## Installation

The system needs to have libjq installed. On OS/X, it comes with jq:

```bash
brew install jq
```

On Linux, use the libjq-dev package:

```bash
apt install libjq-dev
```

Finally, add this to your project's `mix.exs`:

```elixir
def deps do
  [
    {:libjq, "~> 0.1.0"}
  ]
end
```
