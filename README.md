# Libjq

Elixir bindings for libjq, the library form of jq the command-line tool.

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
