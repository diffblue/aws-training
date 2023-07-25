# Incremental SMT2 backend

## Table of content

- Incremental SMT2 backend
- General usage
- Pros
- Documentation and examples
- Future works

## Interactivity requirements

- The training materials (including this slide).
   https://github.com/diffblue/aws-training
- Recent version of CBMC (version 5.88.0).
   https://github.com/diffblue/cbmc/releases
- Recent version of Z3 (4.10.2) (`apt` on ubuntu may be outdated).
   https://github.com/Z3Prover/z3/releases/tag/z3-4.10.2
- Optional: recent version of CVC5 (1.0.0).
   https://github.com/cvc5/cvc5/releases/tag/cvc5-1.0.0

## Incremental SMT2 backend

CBMC comes with a solver-agnostic incremental SMT backend.

The SMT incremental backend supports the following `C` features:

- Integers (via Bit vector arithmetic),
- Pointers,
- Arrays (incl. string literals),
- Structs,
- Unions (partial support),
- Enums.

## General usage

### Requirements:
Any SMT solver that:

- is SMTLib v2.6 compatible;
- accepts formatted input from the standard input;
- supports interactive mode (for incremental solving);
- that supports the `ALL` logic (used to allow quantifiers, bit-vectors, arrays, uninterpreted functions);
- can be executed from the command line.

### Usage
To use this functionality it is enough to add the argument
`--incremental-smt2-solver <cmd>` where `<cmd>` is:

- the command to invoke the SMT solver of choice;
- enables solver incremental mode;
- enables accepting input from the standard input.

*NOTE*: other more solver-specific tuning flags are allowed and left to the user.

Examples of invocations with various solvers (* ):

1. Z3: `cbmc --incremental-smt2-solver 'z3 -smt2 -in' simple.c`
2. CVC5: `cbmc --incremental-smt2-solver 'cvc5 --lang=smtlib2.6 --incremental'  simple.c`
3. Yices: `cbmc --incremental-smt2-solver 'yices-smt2' simple.c`

The new incremental SMT backend has been designed to interoperate with external
solvers, so the solver name must be in the `PATH` or an executable with full
path must be provided.

### Useful extra arguments

The SMTLib commands that are sent to the solver can be shown to the user in 3 possible ways:

1. adding `--verbosity 10` (* ): 
   - logs the problem expressed to the decision procedure `handle` and `set_to` to `stdout`,
   - can be quite cluttered;
2. adding `--outfile <output file>` (* ):
   - creates a SMT2 file with the full formula that would be sent to the solver,
   - does not invoke the solver, so stops at the first `(check-sat)`;
3. adding `--dump-smt-formula <output file>` (* ):
   - creates a SMT2 file with the SMT commands as the solver is interacted with,
   - runs the solver,
   - may contains multiple `(check-sat)` and `(get-value expr)`.

### Compile-time requirements:

*NONE*

The incremental SMT2 backend is solver-agnostic and does not have any extra dependency.

## Pros

### Pros of solver-agnostic backend.

- No compile-time dependencies;
- No license issues;
- Allows to switch the solver at runtime;
- Tuning parameter/flag selection is left to the user;
- Solver is a subprocess communicating uses pipe:
   - allows incremental solving (useful for `--cover location`) (* ),
   - avoids generating huge temporary files.

### Pros of a SMT2 backend compared with SAT.

- Array theory enables better support of large arrays (* ).
- Better support for multiplication and division (** ).

### Pros compared to the old SMT2 backend.

- More modular and structured design:
   - the problems expressed to the decision procedure are translated to an SMT structure (based on `irept`) and only then converted to SMT strings,
   - easier (unit) testing,
   - thoroughly tested,
   - easier maintenance.

## Documentation and examples

- Documentation can be found in `src/solvers/smt2_incremental/README.md`.
- Example files in:
   - `regression/cbmc-incr-smt2`,
   - any regression test in `regression/cbmc` that contains the label `new-smt-backend`.

## Future works:
- Missing features that will come soon:
   - Better union support,
   - Empty struct.
- Better error reporting when initializing solver (planned to be delivered this year),
- Function pointers,
- Floating point numbers,
- CI with solvers different than Z3, CVC5,
- Ad-hoc tuning preambles.


## Decision procedure architecture (Bonus):

### Overview of the sequence of data processing and data flow -

1. Other parts of the cbmc code base send `exprt` based expressions to the
decision procedure through the `handle`, `set_to` and `get` member functions.
See the base class `decision_proceduret` for doxygen of these functions.

2. The `smt2_incremental_decision_proceduret` is responsible for holding state
and building commands to send to the solver. It uses `convert_expr_to_smt` for
the state free (pure) portion of the `exprt` to `termt` conversions.

3. `smt2_incremental_decision_proceduret` sends `smt_commandt` to
`smt_piped_solver_processt`. Although `exprt` is broadly equivalent to `termt`,
the terms must be part of a command giving them a broader meaning before they
are sent to the solver.

4. `smt_piped_solver_processt` uses the `smt_to_smt2_string` function to perform
the conversion from the tree structure of the `smt_commandt` into the linear
structure of the string for sending to the solver.

5. `smt_piped_solver_processt` sends `std::string` to `piped_processt`.

6. `piped_processt` has operating system specific implementations which use
POSIX / Windows API calls to send the strings to the solver process via a pipe.
Note that the solver is kept in a operating system separated process, not a
thread. This supports multiprocessing with the solver ingesting commands whilst
the cbmc process continues symex to generate the following commands.

7. `piped_processt` receives output strings from the solver process using OS API
calls and a buffer, when the `smt_piped_solver_processt` requests them.

8. The response strings returned to `smt_solve_processt` are converted into type
less parse trees in the form of raw `irept`s using `smt2irep`. `smt2irep` is
essentially just an S-expression parser.

9. `smt_piped_solver_processt` uses `validate_smt_response` to convert the
`irept` parse tree into either a set of validation errors or a `smt_responset`.
The case of validation errors is considered to be an error with the solver.
Therefore an exception may be thrown for use as user feedback rather than
violating an `INVARIANT` as would be the case for an internal cbmc error.

10. The well sorted `smt_reponset` is then returned to the
`smt2_incremental_decision_proceduret`.

11. In the case of `smt2_incremental_decision_proceduret::get` the response is
expected to be an `smt_get_value_responset`. The decision procedure uses
`construct_value_expr_from_smt` to convert the value term in the response from
the solver into an expression value. This requires information from the decision
procedure about the kind of type the constructed expression should have. The
reason for this is that the smt formula (although well sorted) does not encode
cbmc's notion of types and a given value in SMT terms could correspond to
multiple different types of cbmc expression.

12. The constructed expression can then be returned to the rest of the cbmc code
base outside the decision procedure.

