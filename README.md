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

```elixir
def deps do
  [
    {:libjq, "~> 0.1.0"}
  ]
end
```
