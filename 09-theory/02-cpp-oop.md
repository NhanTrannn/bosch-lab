# Object-oriented C++

## Theory

### The four OOP principles
- **Encapsulation**: hide data with `private`/`protected`, expose it through public functions.
- **Inheritance**: `public` (is-a), `protected`, `private` (implemented-in-terms-of). struct defaults to public, class to private (for both members and inheritance).
- **Polymorphism**: compile time (overloading, templates) and run time (`virtual` functions).
- **Abstraction**: an abstract class has at least one pure virtual function `virtual void f() = 0;` and cannot be instantiated.

### Constructors / destructors
- Initialization order: virtual bases → bases (in declaration order of inheritance) → members (in **declaration** order in the class, not initializer-list order) → constructor body. Destructors run in reverse.
- `const` members, reference members and members without a default constructor **must** be initialized in the initializer list.
- A single-argument constructor allows implicit conversion; use `explicit` to block it.
- Calling a virtual function inside a constructor/destructor runs the version of the class **currently** being built/destroyed, not the derived one.
- A base class used polymorphically needs a **virtual destructor**, otherwise `delete base_ptr` does not call the derived destructor (UB).

### Special member functions
Default ctor, destructor, copy ctor `T(const T&)`, copy assignment `T& operator=(const T&)`, move ctor `T(T&&)`, move assignment `T& operator=(T&&)`.
- **Rule of three**: if you write one of destructor/copy ctor/copy assignment you usually need all three (resource-owning classes).
- **Rule of five**: add the move ctor and move assignment. **Rule of zero**: use RAII (smart pointers, containers) so you need none of them.
- Shallow copy (copying the pointer) → two objects free the same memory → double free. A deep copy allocates new memory.
- The copy ctor is called when: initializing from another object, passing by value, returning by value (when not elided). Copy assignment: assigning to an object that **already exists**.
- `= default`, `= delete` (e.g. forbid copying: `T(const T&) = delete;`).

### Virtual functions and the vtable
- Each class with virtual functions has a **vtable** (table of function pointers); each object has a **vptr** pointing to its class's vtable.
- Dynamic binding only happens through a **pointer or reference**. Calling through an object (by value) is static binding; assigning a derived object to a base object causes **object slicing**.
- `override` (checks that it really overrides), `final` (forbids further overriding/inheritance).
- Default arguments of a virtual function come from the static type.

### Overload, override, hide
- **Overload**: same name, different parameters, same scope; resolved at compile time. You cannot overload on return type alone.
- **Override**: a derived class redefines a virtual function with the same signature.
- **Hide**: a derived class declares a function with the same name (different parameters, or the base one is not virtual) → it hides **all** base overloads with that name. Bring them back with `using Base::f;`.

### Other keywords
- `static` member: shared by all objects; a static member function has no `this`.
- `const` member function: does not modify members (except `mutable` ones); a const object can only call const functions.
- `friend`: lets another function/class access private members; not inherited, not transitive.
- `inline`, `namespace`, references (`int &r = x;` must be initialized, cannot be reseated, there is no null reference).
- Casts: `static_cast` (compile-time checked conversion), `dynamic_cast` (safe run-time downcast, needs a polymorphic class, returns nullptr / throws `bad_cast`), `const_cast` (removes const), `reinterpret_cast` (reinterprets the bits).
- `new`/`delete` call constructors/destructors; `malloc`/`free` do not. `new[]` must be paired with `delete[]`.

### Exceptions
- `throw` → stack unwinding destroys local objects → the first matching `catch` runs. Catch by `const T&`.
- Destructors are `noexcept` by default; throwing during unwinding → `std::terminate`.
- RAII: a resource is tied to an object's lifetime (lock_guard, unique_ptr, fstream).

## Quiz

### What determines the order in which members are initialized in a constructor?
- [ ] The order in the initializer list
- [x] The order in which the members are declared in the class
- [ ] Alphabetical order of member names
- [ ] The compiler chooses
> Always declaration order. An initializer list in a different order triggers -Wreorder and may read an uninitialized member.

### What does this print?
```cpp
struct B { B(){ cout << "B"; } ~B(){ cout << "~B"; } };
struct D : B { D(){ cout << "D"; } ~D(){ cout << "~D"; } };
int main(){ D d; }
```
- [ ] D B ~B ~D
- [x] B D ~D ~B
- [ ] B D ~B ~D
- [ ] D B ~D ~B
> The base is constructed first and destroyed last.

### What happens when you `delete` a Derived object through a Base pointer and Base's destructor is not virtual?
```cpp
Base *p = new Derived; delete p;
```
- [ ] ~Derived then ~Base are called
- [ ] Only ~Derived is called
- [x] Undefined behavior; in practice usually only ~Base runs, leaking Derived's resources
- [ ] Compile error
> A base class used polymorphically must have a virtual destructor.

### Calling a virtual function inside Base's constructor (while building a Derived object) runs which version?
- [x] Base's version
- [ ] Derived's version
- [ ] Undefined behavior that always crashes
- [ ] Compile error
> While Base is being constructed the Derived part does not exist yet; the vptr points to Base's vtable.

### When does object slicing happen?
- [ ] When assigning a Derived* to a Base*
- [x] When copying a Derived object into a variable of type Base (by value)
- [ ] When using dynamic_cast
- [ ] When calling a virtual function through a reference
> Only the Base part is copied; the Derived part is cut off and virtual calls run Base's version.

### Dynamic binding (calling the derived class's virtual function) only happens when calling through?
- [ ] An object by value
- [x] A pointer or reference to the base class
- [ ] The class name with `::`
- [ ] A static function
> Calling through an object or explicitly with `Base::f()` is static binding.

### What extra data does each object of a class with virtual functions contain?
- [ ] A copy of the vtable
- [x] A vptr pointing to the class's shared vtable
- [ ] A pointer to each virtual function
- [ ] Nothing extra
> There is one vtable per class; each object only adds one vptr (8 bytes on 64-bit) no matter how many virtual functions there are.

### Which declaration makes a class abstract?
- [ ] `virtual void f();`
- [x] `virtual void f() = 0;`
- [ ] `void f() const;`
- [ ] `static void f();`
> A pure virtual function. A derived class must override it before objects can be created.

### When is the copy constructor called? (select all that apply)
- [x] `T b = a;`
- [x] Passing an object by value to `void f(T x)`
- [ ] `b = a;` when b already exists
- [x] `T b(a);`
> `b = a` with an existing b calls the copy assignment operator, not the copy constructor.

### Why must the copy constructor take a reference, `T(const T&)`?
- [ ] To be faster
- [x] Passing by value would itself need the copy constructor → infinite recursion
- [ ] Because the standard forbids const
- [ ] So the source object can be modified
> The compiler rejects `T(T other)`.

### The rule of three says that if you write one of these, you usually need all three:
- [ ] constructor, destructor, operator==
- [x] destructor, copy constructor, copy assignment
- [ ] move constructor, move assignment, destructor
- [ ] constructor, copy constructor, operator<<
> All three deal with resource management. The rule of five adds the move constructor and move assignment.

### How does `void f(double)` in Derived affect Base's `void f(int)`?
```cpp
struct Base { void f(int); };
struct Derived : Base { void f(double); };
Derived d; d.f(1);
```
- [ ] Calls Base::f(int) because it is an exact match
- [x] Calls Derived::f(double), because Derived::f hides every f in Base
- [ ] Ambiguous, compile error
- [ ] Link error
> Name hiding. Add `using Base::f;` in Derived to overload both.

### Can you overload two functions that differ only in their return type?
- [ ] Yes
- [x] No
- [ ] Only if one of them is virtual
- [ ] Only in C++17
> Overload resolution only looks at the parameter list (and the cv/ref qualifiers of member functions).

### What does `dynamic_cast<Derived*>(basePtr)` return when basePtr does not actually point to a Derived?
- [ ] Throws std::bad_cast
- [x] nullptr
- [ ] An invalid pointer (UB)
- [ ] Compile error
> With pointers it returns nullptr; with references it throws `std::bad_cast`. dynamic_cast needs a class with at least one virtual function.

### Which members MUST be initialized in the initializer list? (select all that apply)
- [x] A `const int` member
- [x] A reference member `int &`
- [ ] A `static int` member
- [x] A member object whose class has no default constructor
> Static members are defined outside the class and do not belong to an object.

### What is the `explicit` keyword on a constructor for?
- [ ] Forcing the constructor to be called with `new`
- [x] Blocking implicit conversions through that constructor
- [ ] Allowing the constructor to throw
- [ ] Making the constructor inline
> For example `explicit String(int n)` forbids `String s = 10;`.

### What does `mutable` allow?
- [ ] Modifying a global const variable
- [x] Modifying that member inside a const member function
- [ ] Changing a variable's type at run time
- [ ] Bypassing private access checks
> Typical uses: caches and mutexes inside objects accessed through const functions.

### How do `new` and `malloc` differ? (select all that apply)
- [x] new calls the constructor, malloc does not
- [x] new throws std::bad_alloc on failure, malloc returns NULL
- [ ] malloc returns the right type, no cast needed
- [x] new does not need a byte count
> In C++, malloc returns `void*` and must be cast. new[] must be paired with delete[].

### `class D : B` (no access specifier) is which kind of inheritance?
- [ ] public
- [ ] protected
- [x] private
- [ ] virtual
> The default is private for `class` and public for `struct`.

### What does this print?
```cpp
struct A { virtual void f(int x = 1) { cout << "A" << x; } };
struct B : A { void f(int x = 2) override { cout << "B" << x; } };
A *p = new B; p->f();
```
- [ ] A1
- [x] B1
- [ ] B2
- [ ] A2
> The function is chosen at run time (B::f), but the default argument comes from the static type A → 1.

### How is the diamond problem solved so that there is only one copy of the common base?
- [ ] Using `final`
- [x] `virtual` inheritance
- [ ] Using `friend`
- [ ] Using templates
> `struct B : virtual A`, `struct C : virtual A`, `struct D : B, C` → D contains a single A, and D calls A's constructor directly.
