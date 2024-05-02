defmodule Libjq.NIFTest do
  import Libjq.NIF
  use ExUnit.Case

  test "encode!" do
    assert encode!(nil) == "null"
    assert encode!(false) == "false"
    assert encode!(true) == "true"
    assert encode!(123) == "123"
    assert encode!(4.5) == "4.5"
    assert encode!("abc") == "\"abc\""
    assert encode!([]) == "[]"
    assert encode!([1, 2]) == "[1,2]"
    assert encode!([[1], [2]]) == "[[1],[2]]"
    assert encode!(%{}) == "{}"
    assert encode!(%{"a" => 1}) == "{\"a\":1}"
    assert encode!(%{a: 1}) == "{\"a\":1}"
  end

  test "decode!" do
    assert decode!("null") == nil
    assert decode!("false") == false
    assert decode!("true") == true
    assert decode!("123") == 123
    assert decode!("4.5") == 4.5
    assert decode!("\"abc\"") == "abc"
    assert decode!("[]") == []
    assert decode!("[1]") == [1]
    assert decode!("[1, 2]") == [1, 2]
    assert decode!("{}") == %{}
    assert decode!("{\"a\":1}") == %{"a" => 1}
  end

  test "compile + run" do
    assert compile(".a") |> run("{\"a\":1}") == [1]
    assert compile(".[]") |> run("[1, 2]") == [1, 2]
  end
end
