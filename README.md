# CppUTest for the QP/C (qpc) Real-Time Embedded Framework (library only)

Build and Test status: ![Build and Tests](https://github.com/covemountainsoftware/cpputest-for-qpc/actions/workflows/cmake.yml/badge.svg)

Copyright Matthew Eshleman

If this project inspires your team to select the QP/C (qpc) 
framework for commercial use, please note 
"Matthew Eshleman" or "Cove Mountain Software" in the referral 
field when acquiring a commercial license from Quantum Leaps. Referrals 
encourage and support efforts like this. Thank you!

# Introduction

The `cpputest-for-qpc` library project enables CppUTest for the 
QP/C Real-Time Embedded Framework. This project provides for the 
following capabilities:

* A CppUTest compatible port of the QP/C (qpc) framework, 
  enabling host based unit and integration testing of QP active 
  objects (QActive). 
* Supporting utilities to simplify unit testing of qpc 
  based active objects. Provided classes may also be useful in 
  the final target software, if C++ is also in use.
* A separate examples project providing usage/examples of this library

Benefits of this approach to unit testing active objects include:
* No surprises. The active object under test interacts with the 
  qpc framework, ensuring tested behavior will match target 
  behavior. The test support code provided in this project 
  enables precise control of when an active object executes within 
  the framework.
* No surprises (again). There are no threads. Each test explicitly 
  'gives' CPU time to the active objects under test as needed by 
  each test.
* Accelerated timer related testing. Seconds of 'time' can be 
  tested in microseconds.
* Host based testing is fast. TDD (Test Driven Development) demands
  fast execution of tests.
* Host based testing is easier to use in a continuous integration (CI) 
  system. Run all the tests with every commit. No surprises when 
  it is time to generate a target release.

**An example project demonstrating the features of this library and 
how to import and use from with a CMake build environment is available
here: https://github.com/covemountainsoftware/cpputest-for-qpc-examples**

# Tutorial Video

A tutorial video on each of the features of this library is now available.
See https://youtu.be/hr1qkNH1wSA.

# Environment

This project was developed and proven in Ubuntu, currently targeting 24.04. 
In theory any build or host operating system environment supported by CppUTest will 
be compatible with this code.

## Prerequisites

* CMake and associated build tools were used to develop
  and prove out this project.
* QP/C 
  * You can override the QPC to another directory with your project's exact QPC source code. 
    Define the cmake variable CMS_QPC_TOP_DIR before including the internal CMakeLists.txt. 
  * or:
    * Do not define CMS_QPC_TOP_DIR, and the internal cmake will fetch the appropriate QP/C repo.
* CppUTest (version 4.0) 
* This project requires support for C++14 and C11.

## Continuous Integration

This project has configured GitHub Actions to build and execute all
unit tests found in this project. This is an example
of one of the key benefits of host-based testing of embedded software.

See the configuration at: `.github/workflows/cmake.yml`

# Testing Support

## Restrictions

The CppUTest test support implemented in this project assumes host based 
testing. Therefore, code found in these support files may use standard 
library features requiring heap access, such as `std::vector`. Heap usage is 
oftentimes avoided in firmware or embedded software. Such use is 
restricted to the `cms::test` namespace.

Additionally, the CppUTest environment expects and requires C++ exceptions 
for some test features. Again, these are restricted to the host based testing 
environment, as exceptions are frequently disabled in firmware build targets.

Care was taken to avoid the above in any source code that may ultimately be 
re-used in an embedded project's target build.

## Using with CMake

If a top-level project adds this repository via `add_subdirectory(cpputest-for-qpc)`
then the library larget `cpputest-for-qpc-lib` will be available for linking with 
your CppUTest enabled host based unit-tests. Additionally, the project 
exports a CMake variable `CMS_CMAKE_DIR` with the path to the internal cmake 
folder with various helper cmake files. For example, a unit test target may simply
add: `include(${CMS_CMAKE_DIR}/cpputestCMake.cmake)` to pull in various CppUTest
behavior and build variables. 

Additionally, the top-level project may set the CMake variable `CMS_QPC_TOP_DIR` 
to the project's QP/C directory. This prevents `cpputest-for-qpc-lib` from fetching 
its own copy of QP/C and ensures that both the unit testing environment and the
target microcontroller build are each using the same version of QP/C.

See the examples project for further details: 
https://github.com/covemountainsoftware/cpputest-for-qpc-examples

## Active object interfaces

A typical active object within the QP/C framework may expose the following
interfaces:

* May subscribe to known events published into the framework and react
  to those events.
* May react to events that are posted directly to the active object.
* May expose a direct API, however such APIs traditionally provide only for
  asynchronous behavior (i.e. the API creates an event and posts the event 
  to self.)
* Time: the active object may repeat certain behavior using framework provided
  timers, or may contain timeout related behavior, etc.

Ultimately, the active object is controlling something such
as an external sensor, network device, LED, etc. The following
provides for some illustration.

          Pub/Sub      Direct POST
          Events         Events
            +             +
            |             |
          +--+--------+-----+
          |                 |
          |  Active Object  |
          |                 |
          +-------+---------+
                  |
                  | Direct API calls
                  |
        +------------+-----------------+
        |                              |
        |  Some API / Module / Driver  |
        |                              |
        +------------------------------+

This project provides for methods enabling unit testing of event publish
and subscribing behavior of the active object as well as timer related
behavior, using the exact same interfaces the active object would use
in the production target. CppUTest provides for the mocking capabilities to 
ensure that the active object under test is calling the expected APIs.

Within the associated examples project, please see the tests for `examples/hwLockCtrlService` which
provides examples of:
* Testing for reaction to a published event, where the reaction is observed
  through a CppUTest `mock()`.
* Testing to ensure the active object published an expected event. This project
  provides for functionality (`cms::test::PublishedEventRecorder`) to record all
  events published during a test. The recorded events can be retrieved and verified 
  by the test.
* Testing of time related behavior. This project provides for the ability to
  `MoveTimeForward` within a test.
* Direct POST of events and testing of direct POST responses. See
  the example and search for the Ping/Pong related test, using the
  support class `cms::test::DummyActiveObject`.

## Testing Active Objects using CppUTest

CppUTest support for testing active objects is provided by various files
in the `test_support` subdirectory, especially `test_support/cpputest-qpc-port.`

Of particular interest is the `cms::test:qf_ctrl` module, which 
provides for convenience and helper methods such as:

* `cms::test::qf_ctrl::Setup(...)` - call this from a test's `setup()` method 
  to prepare for active object testing.
* `cms::test::qf_ctrl::Teardown()` - call this from a test's `teardown()` method 
  to perform various actions, including testing for memory pool leaks.
* `cms::test::qf_ctrl::ProcessEvents()` - call this to 'give' some CPU time to 
  any active objects under test. This is a critical feature of this testing
  approach.
* `cms::test::qf_ctrl::MoveTimeForward(...)` - call this to advance 'time',
  potentially activating any internal active object timers. Many seconds,
  minutes, or hours, of time may be tested with this approach in a few 
  milliseconds of host CPU time.
* `cms::test::qf_ctrl::PublishEvent(...)` - convenience method enabling 
  publishing of events from a test.
* `cms::test::qf_ctrl::PublishAndProcess(...)` - additional convenience methods,
  combining publish and process steps. These functions also help to automatically
  ignore a test published event when using a published event recorder.
* `class cms::test::PublishedEventRecorder` - an active object that records
  events published into the framework. Useful when a test expects an
  active object under test to publish an event.

## The basic active object test pattern

To create tests for an active object, the following outline is considered:

1. Implement the `setup()` handler for the test. Initialize the QF framework as 
   desired using `cms::test::qf_ctrl::Setup(...)`.
2. Instantiate or otherwise initialize the active object under test.
3. `start` the active object under test, perhaps as part of the `setup`
   or as a separate step.
4. Prep a single test.
5. Prepare a CppUTest mock() or other resources as needed for the test.
6. Stimulate the unit under test as appropriate. For example, publish an 
   event into the framework that the active object is expected to be subscribed to 
   and expected to respond to in some manner.
7. Give the unit under test some CPU time via `cms::test::qf_ctrl::ProcessEvents()` or
   the equivalent. See also `cms::test::qf_ctrl::MoveTimeForward(...)`.
8. Confirm how the unit under test responded. Did it call the `mock()` as expected? Did
   it publish a response into the framework? What data do I need to inspect for changes?

That is it! That is the basic pattern for testing an active object. See  
examples at: `examples/hwLockCtrlService/test/hwLockCtrlServiceTests.cpp` to
learn more.

Other tips:
* Avoid internal state knowledge as much as possible. This allows for internal
  state machine refactoring without impacting the tests. 
* Follow best practices in your test code, especially follow the DRY principle.

# Acknowledgements

I must take a moment to thank key influences that inspired or helped
enable this project:

* Miro Samek and his book and frameworks. 
    Learn more at: https://www.state-machine.com/
* James Grenning and his book on TDD. 
    Learn more at: https://wingman-sw.com/
* The CppUTest project: https://cpputest.github.io/

# License

All code in this project found in the `cms` namespace follows a dual-license approach.
Please see LICENSE.txt for details.

All licenses for external source code and libraries relied upon by this project 
remain fully owned by their respective owners. In particular, please
see the licensing details for qpc at: https://www.state-machine.com/licensing.

If this project inspires your team to select the qpc framework for commercial 
use, please note "Matthew Eshleman" or "Cove Mountain Software" in the referral
field when acquiring a commercial license from Quantum Leaps. Referrals encourage 
and support this effort. Thank you!

# References

This project is the QP/C port of an earlier equivalent effort for QP/C++. 
See `cpputest-for-qpcpp` at: https://github.com/covemountainsoftware/cpputest-for-qpcpp

This project was also inspired by a non-qpc example, see this blog post:
https://covemountainsoftware.com/2020/04/17/unit-testing-active-objects-and-state-machines/

Additionally, please see that post's associated github repo:
https://github.com/covemountainsoftware/activeObjectUnitTestingDemo

Other references:
* Sutter, Herb. Prefer Using Active Objects Instead of Naked Threads. Dr. Dobbs, June 2010.
* Grenning, James. Test Driven Development for Embedded C.
* Samek, Miro. Practical UML Statecharts in C/C++: Event-Driven Programming for Embedded Systems.
