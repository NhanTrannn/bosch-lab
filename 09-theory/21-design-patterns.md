# Design patterns

## Theory

Covers the patterns in the tutorialspoint "Design Patterns" tutorial (the 23 GoF patterns plus the J2EE patterns), with C++ in mind. Labs: `11-design-patterns/01` … `12`.

### What design patterns are
- A design pattern is a proven, reusable solution to a common problem in object-oriented design — a template, not finished code. Patterns give developers a **shared vocabulary** ("use an observer here") and capture **best practices** learned by trial and error.
- **Gang of Four (GoF)**: Erich Gamma, Richard Helm, Ralph Johnson and John Vlissides, authors of *Design Patterns: Elements of Reusable Object-Oriented Software* (1994), which describes **23 patterns**.
- The two principles the GoF patterns rest on:
  - **Program to an interface, not an implementation** (callers depend on abstract types).
  - **Favor object composition over class inheritance** (combine objects at run time instead of building deep hierarchies).
- Related principles often asked about: **SOLID** — Single responsibility, Open/closed (open for extension, closed for modification), Liskov substitution, Interface segregation, Dependency inversion (depend on abstractions).

### Categories
| Category | Concern | Patterns |
| --- | --- | --- |
| **Creational** | how objects are created, hiding the creation logic instead of calling `new` directly | Factory, Abstract Factory, Singleton, Builder, Prototype |
| **Structural** | how classes and objects are composed into larger structures | Adapter, Bridge, Filter (Criteria), Composite, Decorator, Facade, Flyweight, Proxy |
| **Behavioral** | communication and responsibilities between objects | Chain of Responsibility, Command, Interpreter, Iterator, Mediator, Memento, Observer, State, Null Object, Strategy, Template Method, Visitor |
| **J2EE** (presentation/business/integration tiers, identified by Sun Java Center) | enterprise application architecture | MVC, Business Delegate, Composite Entity, Data Access Object, Front Controller, Intercepting Filter, Service Locator, Transfer Object |

(Filter and Null Object are not among the original 23 GoF patterns but are covered by the tutorial.)

### Creational patterns
| Pattern | Intent | C++ sketch / typical use |
| --- | --- | --- |
| **Factory** | create an object without exposing the creation logic; refer to it through a common interface | `unique_ptr<Shape> ShapeFactory::create("CIRCLE")` |
| **Factory Method** | a base class defines an interface for creating an object, subclasses decide which class to instantiate | virtual `createButton()` called by a non-virtual `show()` |
| **Abstract Factory** | a factory of factories: create **families** of related objects without naming concrete classes | `GuiFactory` → `WinFactory`, `LinuxFactory` (button + checkbox) |
| **Singleton** | ensure a class has only **one instance** and provide a global access point | private ctor, deleted copy, `static T &instance() { static T t; return t; }` (thread-safe since C++11) |
| **Builder** | build a complex object step by step; the same process can create different representations | meal builder, fluent `RequestBuilder().url(..).header(..).build()` |
| **Prototype** | create new objects by **cloning** an existing one | `virtual unique_ptr<Base> clone() const` ("virtual constructor"), deep copy |

### Structural patterns
| Pattern | Intent | Typical use |
| --- | --- | --- |
| **Adapter** | convert one interface into another the client expects (a bridge between incompatible interfaces) | wrapping a legacy library; object adapter (composition) or class adapter (inheritance) |
| **Bridge** | decouple an abstraction from its implementation so both vary independently | `Shape` holds a `DrawAPI*`; avoids m × n subclasses |
| **Filter / Criteria** | filter a set of objects with criteria that can be combined by logical operations | `AndCriteria(single, male).meet(people)` |
| **Composite** | compose objects into tree structures and treat individual objects and groups uniformly | file systems, GUI containers, org charts |
| **Decorator** | add responsibilities to an object **dynamically** by wrapping it in an object with the same interface | stream wrappers, coffee add-ons, middleware |
| **Facade** | provide a simple unified interface to a complex subsystem | `computer.start()` hides CPU/memory/disk steps |
| **Flyweight** | share objects to support large numbers of fine-grained objects efficiently (intrinsic state shared, extrinsic state passed in) | glyphs in a text editor, particles, map tiles |
| **Proxy** | a surrogate that controls access to another object | virtual proxy (lazy loading), protection proxy, remote proxy, caching proxy, smart pointers |

### Behavioral patterns
| Pattern | Intent | Typical use |
| --- | --- | --- |
| **Chain of Responsibility** | pass a request along a chain of handlers; decouple the sender from the receivers | logger levels, event bubbling, middleware |
| **Command** | encapsulate a request as an object (execute/undo) | undo/redo, job queues, GUI actions, macros |
| **Interpreter** | represent a grammar as classes and evaluate sentences of a small language | rule engines, expression evaluators, SQL/regex parsing |
| **Iterator** | access elements of a collection sequentially without exposing its representation | STL iterators, `begin()/end()` |
| **Mediator** | centralize communication between objects to reduce many-to-many coupling | chat rooms, air-traffic control, dialog boxes |
| **Memento** | capture and restore an object's internal state without breaking encapsulation | undo, snapshots, checkpoints (roles: originator, memento, caretaker) |
| **Observer** | one-to-many dependency: when one object changes, its dependents are notified | event systems, MVC views, publish/subscribe |
| **State** | an object changes its behavior when its internal state changes (looks like it changed class) | state machines: turnstiles, TCP connections, vending machines |
| **Null Object** | replace null with an object that does nothing, avoiding null checks | default loggers, "not found" customers |
| **Strategy** | define a family of interchangeable algorithms and choose one at run time | sorting/compression strategies, payment methods, `std::function` callbacks |
| **Template Method** | define the skeleton of an algorithm in a base class and let subclasses override steps | frameworks with hooks: `Game::play()` calls `initialize/start/end` |
| **Visitor** | add new operations to an object structure without changing its classes (double dispatch) | compilers walking an AST, exporting/pricing parts |

### J2EE patterns
| Pattern | Intent |
| --- | --- |
| **MVC** | separate Model (data + business rules), View (presentation) and Controller (handles input, updates the model and the view) |
| **Business Delegate** | decouple the presentation tier from business services; hides lookup and access details |
| **Composite Entity** | a coarse-grained entity managing a graph of dependent objects, persisted together |
| **Data Access Object (DAO)** | separate low-level data access (database, file) from business logic behind an interface |
| **Front Controller** | a single handler for all requests (authentication, logging, dispatching to views) |
| **Intercepting Filter** | pre/post-process requests through a chain of filters before the target handler |
| **Service Locator** | locate services through a central registry and **cache** them (lookups are expensive) |
| **Transfer Object (DTO)** | a simple serializable object that carries data between layers in one call |

### Commonly confused pairs
- **Factory Method vs Abstract Factory**: one product created by a subclass method vs whole families of products created by a factory object.
- **Adapter vs Decorator vs Proxy vs Facade**: Adapter *changes* the interface; Decorator *keeps* the interface and adds behavior; Proxy *keeps* the interface and controls access; Facade *simplifies* a whole subsystem behind a new interface.
- **Adapter vs Bridge**: Adapter is applied after the fact to make existing classes fit; Bridge is designed up front to separate abstraction and implementation.
- **Strategy vs State**: same structure; the client picks the strategy, whereas states replace themselves.
- **Strategy vs Template Method**: composition (swap the whole algorithm object) vs inheritance (override steps of a fixed algorithm).
- **Composite vs Decorator**: both are recursive; Composite has many children and aggregates them, Decorator has one wrapped object and adds behavior.
- **Observer vs Mediator**: Observer broadcasts state changes to subscribers; Mediator coordinates interactions between colleagues.
- **Singleton drawbacks**: hidden global state, tight coupling, hard to unit test; thread-safety of lazy initialization (use a function-local static in C++11+).

## Quiz

### What is a design pattern?
- [ ] A library of ready-made classes
- [x] A general, reusable solution to a commonly occurring problem in software design
- [ ] A coding style guide for naming variables
- [ ] A UML diagram type
> A pattern describes the problem, the solution structure and its consequences; it is a template, not finished code.

### Who are the "Gang of Four"?
- [ ] The four creators of C++
- [x] Gamma, Helm, Johnson and Vlissides, authors of the 1994 book "Design Patterns: Elements of Reusable Object-Oriented Software"
- [ ] The four founders of the Unix project
- [ ] The authors of the SOLID principles
> Their book catalogs 23 patterns in three categories.

### How many patterns does the GoF book describe?
- [ ] 12
- [ ] 17
- [x] 23
- [ ] 33
> 5 creational, 7 structural and 11 behavioral patterns.

### Which two principles do the GoF patterns rest on? (select all that apply)
- [x] Program to an interface, not an implementation
- [x] Favor object composition over class inheritance
- [ ] Always use multiple inheritance
- [ ] Prefer global variables for shared state
> Depending on abstractions and composing objects at run time keeps designs flexible.

### Which category do creational patterns belong to?
- [x] Ways to create objects while hiding the creation logic instead of instantiating them directly with new
- [ ] Ways to compose classes and objects into larger structures
- [ ] Ways objects communicate and share responsibilities
- [ ] Patterns for the presentation tier of enterprise applications
> Structural = composition, behavioral = communication, J2EE = enterprise tiers.

### Which of these are structural patterns? (select all that apply)
- [x] Adapter
- [x] Composite
- [ ] Observer
- [x] Proxy
> Observer is behavioral. Other structural patterns: Bridge, Decorator, Facade, Flyweight, Filter.

### Which of these are behavioral patterns? (select all that apply)
- [x] Strategy
- [x] Visitor
- [ ] Builder
- [x] Chain of Responsibility
> Builder is creational.

### What does the Singleton pattern guarantee?
- [ ] Every object has a unique ID
- [x] A class has only one instance, with a global point of access to it
- [ ] A class cannot be inherited
- [ ] Objects are created only on the stack
> Usually a private constructor, deleted copy operations and a static accessor.

### Which is the simplest thread-safe lazy Singleton in C++11 and later?
- [ ] `static T *p; if (!p) p = new T; return *p;`
- [x] `static T &instance() { static T t; return t; }`
- [ ] A global `T t;` in a header
- [ ] `return *new T;` on every call
> Initialization of a function-local static is guaranteed thread-safe since C++11 (the "Meyers singleton"). The first option is a data race; the last creates many objects and leaks.

### Why is the Singleton's copy constructor deleted (or private)?
- [ ] To make it faster
- [x] So nobody can create a second instance by copying the existing one
- [ ] Because singletons cannot have constructors
- [ ] To allow inheritance
> `T(const T&) = delete; T& operator=(const T&) = delete;`.

### Which is a commonly cited drawback of Singleton?
- [ ] It uses too much memory
- [x] It introduces hidden global state and makes code harder to unit test
- [ ] It cannot be used in C++
- [ ] It always needs a mutex for every call
> Many designs prefer passing dependencies explicitly (dependency injection).

### How does Abstract Factory differ from Factory Method?
- [ ] They are the same pattern
- [x] Abstract Factory creates families of related objects through a factory object; Factory Method lets a subclass decide which single product to create
- [ ] Factory Method creates families, Abstract Factory creates one object
- [ ] Abstract Factory is structural
> Abstract Factory is sometimes called a "factory of factories".

### A class has 10 optional constructor parameters, leading to many overloaded constructors. Which pattern helps?
- [ ] Prototype
- [x] Builder
- [ ] Adapter
- [ ] Flyweight
> A builder sets parts step by step (often fluently) and builds the object at the end, avoiding "telescoping constructors".

### What does the Prototype pattern use to create objects?
- [ ] A factory method per class
- [x] Cloning an existing object, usually through a virtual clone()
- [ ] A global registry of constructors only
- [ ] Copying the class definition at compile time
> Useful when creation is expensive or the concrete type is only known at run time. clone() should perform a deep copy when the object owns resources.

### Which pattern converts the interface of a class into another interface clients expect?
- [x] Adapter
- [ ] Bridge
- [ ] Decorator
- [ ] Facade
> Adapter lets incompatible classes work together, e.g. wrapping a legacy API.

### What is the goal of the Bridge pattern?
- [ ] To connect two networks
- [x] To decouple an abstraction from its implementation so that the two can vary independently
- [ ] To add behavior to objects at run time
- [ ] To create families of objects
> The abstraction holds a pointer to an implementation interface, avoiding a class for every abstraction × implementation combination.

### Which pattern lets clients treat individual objects and compositions of objects uniformly in a tree?
- [ ] Decorator
- [x] Composite
- [ ] Flyweight
- [ ] Iterator
> Leaves and composites share one interface; operations recurse through the tree.

### Which pattern adds responsibilities to an object dynamically, without subclassing, by wrapping it?
- [ ] Adapter
- [ ] Proxy
- [x] Decorator
- [ ] Composite
> A decorator has the same interface as the wrapped object and can be stacked (e.g. milk + sugar on a coffee).

### Which pattern provides a simplified interface to a complex subsystem?
- [x] Facade
- [ ] Mediator
- [ ] Proxy
- [ ] Bridge
> The client calls one high-level method; the facade coordinates the subsystem's classes.

### Flyweight saves memory by:
- [ ] Compressing objects
- [x] Sharing objects: intrinsic state is shared and cached, extrinsic state is supplied by the client
- [ ] Storing objects on disk
- [ ] Using smaller data types
> E.g. one shared Circle per color, with the position passed in on each draw.

### An image object is only loaded from disk the first time display() is called. Which pattern is that?
- [ ] Decorator
- [x] Proxy (virtual proxy)
- [ ] Facade
- [ ] Prototype
> A proxy has the same interface as the real object and controls access to it: lazy loading, access control, remote access, caching.

### Which statement correctly distinguishes Adapter, Decorator and Proxy?
- [x] Adapter changes the interface; Decorator keeps it and adds behavior; Proxy keeps it and controls access
- [ ] All three change the interface
- [ ] Decorator changes the interface; Adapter adds behavior
- [ ] Proxy creates new objects; Adapter destroys them
> All three wrap another object, but for different reasons.

### Which pattern passes a request along a chain of handlers until it is handled?
- [ ] Command
- [x] Chain of Responsibility
- [ ] Mediator
- [ ] Iterator
> The sender does not know which handler will process the request; e.g. INFO/DEBUG/ERROR loggers.

### Which pattern encapsulates a request as an object, making undo/redo and request queues possible?
- [ ] Memento
- [x] Command
- [ ] Strategy
- [ ] State
> A command object has execute() (and often undo()); an invoker stores and runs commands without knowing what they do.

### Which pattern captures an object's internal state so it can be restored later, without violating encapsulation?
- [ ] Prototype
- [ ] Command
- [x] Memento
- [ ] State
> Roles: originator (creates/restores), memento (opaque snapshot), caretaker (stores mementos without looking inside).

### Which pattern defines a one-to-many dependency so that when one object changes state, all its dependents are notified?
- [x] Observer
- [ ] Mediator
- [ ] Chain of Responsibility
- [ ] Visitor
> Also called publish/subscribe. Observers must be detached (or held as weak_ptr) before they are destroyed.

### Which pattern lets you choose an algorithm at run time from a family of interchangeable algorithms?
- [ ] Template Method
- [x] Strategy
- [ ] Visitor
- [ ] Interpreter
> The context holds a strategy object (or a std::function) and delegates the work to it.

### How does State differ from Strategy?
- [ ] They have completely different structures
- [x] The structure is similar, but with State the state objects switch the context's state themselves as events happen, while with Strategy the client chooses the algorithm
- [ ] State is creational, Strategy is structural
- [ ] Strategy can only be used once
> State models a state machine; Strategy models interchangeable algorithms.

### What is characteristic of the Template Method pattern?
- [ ] Every step is chosen at run time by the client
- [x] A base-class method defines the fixed skeleton of an algorithm and calls steps that subclasses override
- [ ] It creates objects from templates (C++ templates)
- [ ] It copies a prototype object
> The skeleton method is usually non-virtual; overridable steps may have default implementations (hooks).

### Which pattern provides sequential access to a collection's elements without exposing its underlying representation?
- [x] Iterator
- [ ] Visitor
- [ ] Composite
- [ ] Facade
> In C++, providing begin()/end() makes a class usable in range-based for loops.

### What is the main benefit of the Visitor pattern?
- [ ] Adding new element classes is easy
- [x] Adding new operations on an object structure without changing the element classes (using double dispatch)
- [ ] Reducing memory usage
- [ ] Hiding a subsystem
> The trade-off: adding a new element type requires changing every visitor.

### Which pattern reduces direct many-to-many communication between objects by routing it through a central object?
- [ ] Observer
- [x] Mediator
- [ ] Facade
- [ ] Proxy
> Example: users send messages to a chat room, which forwards them to the other users.

### Which pattern evaluates sentences of a simple language by representing its grammar as a class hierarchy?
- [ ] Visitor
- [ ] Composite
- [x] Interpreter
- [ ] Command
> Terminal and non-terminal expressions (e.g. Or, And) each implement interpret(context).

### What problem does the Null Object pattern solve?
- [ ] Freeing memory automatically
- [x] Avoiding repeated null checks by returning an object with neutral "do nothing" behavior instead of null
- [ ] Making objects immutable
- [ ] Preventing inheritance
> e.g. findCustomer() returns a NullCustomer whose name() says "not available".

### In MVC, which component handles user input and updates the model and the view?
- [ ] Model
- [ ] View
- [x] Controller
- [ ] Data Access Object
> Model = data and business rules, View = presentation, Controller = input handling and coordination.

### What is the purpose of the Data Access Object (DAO) pattern?
- [ ] Caching service lookups
- [x] Separating low-level data access operations (database, files) from business logic behind an interface
- [ ] Carrying data between layers
- [ ] Handling all incoming web requests
> The business code calls getAll/update/delete without knowing how the data is stored.

### What is a Transfer Object (DTO)?
- [x] A simple object that carries data between tiers/processes in one call
- [ ] An object that transfers ownership of memory
- [ ] A network socket wrapper
- [ ] A proxy for a remote object
> It has fields (and getters/setters) but no business logic.

### What does the Service Locator pattern typically add on top of a plain service lookup?
- [ ] Encryption
- [x] Caching of looked-up services, because lookups (e.g. JNDI) are expensive
- [ ] Multithreading
- [ ] Automatic code generation
> The first request goes to the registry; later requests for the same service come from the cache.

### Which J2EE pattern provides a single entry point that handles all requests (authentication, logging, dispatching)?
- [ ] Intercepting Filter
- [x] Front Controller
- [ ] Business Delegate
- [ ] Composite Entity
> An Intercepting Filter runs pre/post-processing filters around a request; a Business Delegate decouples the presentation tier from business services.

### Which SOLID principle says software entities should be open for extension but closed for modification?
- [ ] Single responsibility
- [x] Open/closed
- [ ] Liskov substitution
- [ ] Dependency inversion
> Strategy, Decorator and Factory are classic ways to add behavior without editing existing classes.

### Which pattern pairs are both about wrapping an object? (select all that apply)
- [x] Decorator
- [x] Proxy
- [x] Adapter
- [ ] Singleton
> They differ in intent: add behavior, control access, convert interface. Singleton is about limiting the number of instances.
