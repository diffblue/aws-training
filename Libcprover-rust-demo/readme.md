# Demonstration/Tutorial of the CBMC Rust API

Hello everyone.

Today we're going to demonstrate how one can go about using the Libcprover-rust
API to integrate with CBMC at an FFI level instead of using CBMC as a binary.

Libcprover Rust is hosted under the `src/libcprover-rust` subfolder inside the
CBMC repository. For a user who wishes to integrate with it, it's also available
as a crate, at [`crates.io: Libcprover-rust`](https://crates.io/crates/libcprover_rust).

Next, we're going to show how you can interact with the project within the
`cbmc` repository (such as, run its tests) before we move on to how to integrate
it in another repository as a user.

## Interacting with `src/libcprover-rust/` within CBMC

The first step before any interaction with the API itself, is to build CBMC.
We depend on the static library artifact that encompases all of CBMC for now
(`libcprover-x.y.z.a`), as the Rust end needs to be able to hook into the
C++ API exposed by CBMC, which from their end depend on the internal APIs of
CBMC to be able to offer their services (analysis, loading of Goto Programs, etc).

We will start with a new, clean slate repository to demonstrate that:

```sh
$ git clone --depth 1 --branch cbmc-5.88.0 git@github.com:diffblue/cbmc.git
$ cd cbmc && cmake -S. -Bbuild -DWITH_JBMC=OFF && cmake --build build -j10 
```

After we've built CBMC, it's a good practice to make sure that the library
we need is present in the build artefacts folder:

```sh
$ ls cbmc/build/lib
[...]
libcprover.5.88.0.a
[...]
```

Now that we've verified this, we're ready to start playing around with the
Rust API.

For starters, let's go there:

```sh
$ cd cbmc/src/libcprover-rust
$ ls
Cargo.toml build.rs   include    other      readme.md  src
```

The source code implementing the library is available under `libcprover-rust/src/`,
in the `lib.rs` and `c_api.cc` file.

The `lib.rs` file contains the implementation of the FFI bridge, along with its
tests. The `c_api.cc` is a shim - it contains initialisation code, utility functions
and translation functions for massaging types into the correct form before they cross
the FFI boundary. This is necessary in cases where there's a small divergence between
the API exposed by the C++ API, and the types expected by the Rust library functions.

Folder `include/` contains headers that are there to assist the Rust/C++ compiler
when they build the FFI bridge. They contain code that handles exception handling,
the signatures of utility functions defined in the shim, etc.

Folder `other/` was created to hold any necessary files that we didn't have where else
to put. For now, it hosts a C file that we use to exercise the unit tests of the API.

To build the `libcprover-rust` project, we need to set up three environment variables
(or pass them inline during the `cargo` call):

* `CBMC_INCLUDE_DIR`, which denotes the include directory, where `cargo` is going to
  look for the C++ headers that the Rust API needs to build with,
* `CBMC_LIB_DIR`, denoting the folder that contains the `libcprover-x.y.z.a` static library
  we need to link with, and
* `CBMC_VERSION`, a string containing the version of CBMC we're building against (this is
  then used to determine which values to substitute for `x.y.z` in the library identifier -
  this is acting as a selector in case the folder contains many different versions of
  CBMC installed side by side).

Let's build in a way that works for my local configuration:

```sh
$ CBMC_INCLUDE_DIR=CBMC_INCLUDE_DIR=../libcprover-cpp/ CBMC_LIB_DIR=../../build/lib CBMC_VERSION=5.88.0 cargo build
Compiling proc-macro2 v1.0.66
   Compiling cc v1.0.79
   Compiling unicode-ident v1.0.11
   Compiling scratch v1.0.7
   Compiling unicode-width v0.1.10
   Compiling cxxbridge-flags v1.0.102
   Compiling termcolor v1.2.0
   Compiling once_cell v1.18.0
   Compiling codespan-reporting v0.11.1
   Compiling link-cplusplus v1.0.9
   Compiling cxx v1.0.102
   Compiling quote v1.0.32
   Compiling syn v2.0.27
   Compiling cxx-build v1.0.102
   Compiling cxxbridge-macro v1.0.102
   Compiling libcprover_rust v5.88.0 (/Users/fotis/Devel/AWS_Training/Libcprover-rust-demo/cbmc/src/libcprover-rust)
    Finished dev [unoptimized + debuginfo] target(s) in 3.81s
```

Of course, just building the API isn't terribly useful on its own (aside from communicating
that it can build, and we haven't made any mistakes in our code during modification that
would interfere with its building).

So what we may want to do most of the time is to run the tests the API comes with. To do
that, we will have a similar invocation to our previous one, only that instead of `build`
we're going to ask `cargo` to `test`.

```sh
CBMC_INCLUDE_DIR=../libcprover-cpp/ CBMC_LIB_DIR=../../build/lib CBMC_VERSION=5.88.0 cargo test -- --test-threads=1 --nocapture
   Compiling libcprover_rust v5.88.0 (/Users/fotis/Devel/AWS_Training/Libcprover-rust-demo/cbmc/src/libcprover-rust)
warning: unused variable: `status`
   --> src/lib.rs:520:24
    |
520 |             if let Err(status) = prop_status {
    |                        ^^^^^^ help: if this is intentional, prefix it with an underscore: `_status`
    |
    = note: `#[warn(unused_variables)]` on by default

warning: `libcprover_rust` (lib test) generated 1 warning (run `cargo fix --lib -p libcprover_rust --tests` to apply 1 suggestion)
    Finished test [unoptimized + debuginfo] target(s) in 0.46s
     Running unittests src/lib.rs (target/debug/deps/libcprover_rust-65042c1e82f1d709)

running 12 tests
test tests::it_can_drop_unused_functions_from_model ... ok
test tests::it_can_get_the_property_description_for_existing_property ... ok
test tests::it_can_get_the_property_status_for_existing_property ... ok
test tests::it_can_load_model_from_file ... ok
test tests::it_can_produce_verification_results_for_file ... ok
test tests::it_can_query_property_identifiers_from_result ... ok
test tests::it_can_verify_the_loaded_model ... ok
test tests::it_raises_an_exception_when_getting_status_of_non_existing_property ... ok
test tests::it_raises_an_exception_when_getting_the_property_description_for_nonexisting_property ... ok
test tests::it_translates_exceptions_to_errors ... ok
test tests::it_works ... ok
test tests::translate_vector_of_rust_string_to_cpp ... ok

test result: ok. 12 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.25s
```

This builds and runs all of our tests - and we can see all of them passing.

The invocation for testing is similar to building, in that it requires the three environment
variables listed before (as part of running the tests requires building a binary that contains
the test code).

What's different this time though, is the test command `cargo test -- --test-threads=1 --nocapture`,
which we can explain as follows:

* `cargo test` - run the project tests
* `--` - everything that follows is passed as argument to the test binary, interpreted by
  the harness that `cargo` builds into the test binary when building a test configuration
* `--test-threads=1` - by default the test-binary harness runs with as many threads as it
  can based on the local machine. However, CBMC is not built into a thread safe way, so we
  need to limit the number of threads so we don't get any concurrency errors from CBMC's end.
* `--nocapture` - tells harness to pass through writes to `stdout` instead of intercepting it
  (useful when debugging - you can ignore it if you just want to exercise the test suite).

## Integrating/Using Libcprover-rust

What we saw before is the interaction base for a developer of `libcprover-rust` (or someone
who wants to modify it before building/integrating it in his own project).

How would one go about using it however, without changing its source code? In other words,
is there any way to integrate it the same way someone might integrate with any other Rust library?

As you can probably guess, the answer to this question is positive.

In the beginning of this tutorial, we mentioned that the library is published as
a crate in `crates.io`. Indeed, the project's page in `crates.io` can be found
at [`crates.io: Libcprover-rust`](https://crates.io/crates/libcprover_rust), and
integration is as easy as any other Rust crate.

For the rest of this tutorial, we're going to build a tool called `minikani`, which is
going to be a miniature kani/cbmc, based on the APIs we expose already as part of the C++
API of CBMC. Our project is going to exclusively use Rust code, and no C++ code (we're
going to use `Libcprover-rust`, which underneath hooks into `Libcprover-cpp`).

As before, the first thing we're actually going to occupy ourselves with is to install
the dependencies for the Rust project - as a reminder, we depend on the whole of CProver
as a static library - `libcprover-x.y.z.a` - and the headers that the compilers depend
on while building the Rust FFI bridge.

One possible way as we saw before is to build CBMC locally. Another way to make sure
these dependencies are installed is through a distribution's package manager. In my
case, I'm going to use homebrew to install `cbmc.5.88.0`:

```sh
$ brew install cbmc@5.88.0
[...]
$ whereis cbmc
cbmc: /opt/homebrew/bin/cbmc /opt/homebrew/share/man/man1/cbmc.1
$ /opt/homebrew/Cellar/cbmc@5.88.0/5.88.0/libexec/lib
[...] libcprover.5.88.0.a
$ ls /opt/homebrew/Cellar/cbmc@5.88.0/5.88.0/include/cprover
api.h                 api_options.h         verification_result.h
```

Excellent. With the dependencies now installed, I'm going to go ahead and create
a new rust project:

```sh
$ cargo new minikani
Created binary (application) `minikani` package
```

Now, with our new project in place, we want to add our dependency, and build our
project to make sure that no compilation problems exist and everything works as
we would expect:

```sh
$ cd minikani
$ cargo add libcprover_rust@5.88.0
$ cargo run
  Downloaded libcprover_rust v5.88.0
  Downloaded 1 crate (10.8 KB) in 0.89s
   Compiling proc-macro2 v1.0.66
   Compiling cc v1.0.79
   Compiling unicode-ident v1.0.11
   Compiling scratch v1.0.7
   Compiling cxxbridge-flags v1.0.102
   Compiling unicode-width v0.1.10
   Compiling termcolor v1.2.0
   Compiling once_cell v1.18.0
   Compiling codespan-reporting v0.11.1
   Compiling link-cplusplus v1.0.9
   Compiling cxx v1.0.102
   Compiling quote v1.0.32
   Compiling syn v2.0.27
   Compiling cxx-build v1.0.102
   Compiling cxxbridge-macro v1.0.102
   Compiling libcprover_rust v5.88.0
error: failed to run custom build command for `libcprover_rust v5.88.0`

Caused by:
  process didn't exit successfully: `/Users/fotis/Devel/AWS_Training/Libcprover-rust-demo/minikani/target/debug/build/libcprover_rust-7c430bb2b40894d0/build-script-build` (exit status: 101)
  --- stderr
  thread 'main' panicked at 'Error: Environment variable `CBMC_INCLUDE_DIR' not set.
   Advice: Please set the environment variable `CBMC_INCLUDE_DIR' with the path that contains cprover/api.h on your system.', /Users/fotis/.cargo/registry/src/github.com-1ecc6299db9ec823/libcprover_rust-5.88.0/build.rs:67:13
  note: run with `RUST_BACKTRACE=1` environment variable to display a backtrace
warning: build failed, waiting for other jobs to finish...
```

Ooops. It seems like we forgot to provide our environment variables. This is also
hinted at by the error message during the compilation:

```
'Error: Environment variable `CBMC_INCLUDE_DIR' not set.
Advice: Please set the environment variable `CBMC_INCLUDE_DIR' with the path that contains cprover/api.h on your system.'
```

Let's try to point the build system to the folders we identified previously as
containing our dependencies:

```sh
$ CBMC_INCLUDE_DIR=/opt/homebrew/Cellar/cbmc@5.88.0/5.88.0/include/cprover/ CBMC_LIB_DIR=/opt/homebrew/Cellar/cbmc@5.88.0/5.88.0/libexec/lib CBMC_VERSION=5.88.0 cargo run
Compiling libcprover_rust v5.88.0
   Compiling minikani v0.1.0 (/Users/fotis/Devel/AWS_Training/Libcprover-rust-demo/minikani)
    Finished dev [unoptimized + debuginfo] target(s) in 0.96s
     Running `target/debug/minikani`
Hello, world!
```

Awesome. This means that we have built our binary, include `libcprover-rust`. But...
our binary doesn't use the library yet, and doesn't do anything excitement generating
yet (there's only so much excitement we can get from a `Hello, world!`).

Let's make it a bit more exciting. Let's change our `main.rs`:

```rust
use libcprover_rust::cprover_api;
use libcprover_rust::ffi_util;
use std::process;

fn main() {
    let client = cprover_api::new_api_session();

    let vec: Vec<String> = vec!["/tmp/example.c".to_owned()];

    let vect = ffi_util::translate_rust_vector_to_cpp(vec);

    if let Err(err) = client.load_model_from_files(vect) {
        eprintln!("Failed to load model from files: {:?} {:?}", vect, err);
        process::exit(1);
    }

    // Validate integrity of goto-model
    if let Err(_) = client.validate_goto_model() {
        eprintln!("Failed to validate goto model from files: {:?}", vect);
        process::exit(1);
    }

    if let Err(_) = client.verify_model() {
        eprintln!("Failed to verify model from files: {:?}", vect);
        process::exit(1);
    }

    let msgs_cpp = cprover_api::get_messages();
    let msgs_rust = ffi_util::translate_cpp_vector_to_rust(msgs_cpp);
    ffi_util::print_response(&msgs_rust);
}
```
