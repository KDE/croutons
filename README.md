# Croutons

Croutons is a library that aims to make working with asynchronous code in a Qt context easy, and in particular, making asynchronous code with QML easy.

## Futures

Croutons has three future types: `FutureBase`, `Future<T> : FutureBase`, `FutureResult<T, Error> : FutureBase`.

## Coroutines

Croutons has integration with C++20 coroutines: simply include `coroutine_integration.h` to make it so that a coroutine can return a future from Croutons.

Croutons also has integration for various Qt types, allowing you to `co_await` them in a coroutine.
