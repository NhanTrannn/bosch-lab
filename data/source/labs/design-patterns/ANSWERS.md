# Design patterns — answers

> Open this file only **after** you have predicted the output yourself. Labs whose output is **nondeterministic** are marked ⚠️.

Design patterns in C++, following the list of patterns covered by the tutorialspoint "Design Patterns" tutorial (GoF creational, structural and behavioral patterns, plus the J2EE patterns). The code and explanations are original.

### 01_singleton — Singleton: lazy Meyers singleton, deleted copy, thread-safe initialization

**Question:** Is "Config() constructed" printed before or after "main starts"? How many times is it printed when 4 threads call instance() at once? Are a and b the same object? Does "Eager() constructed" appear before main?

**Expected output:**

```text
Eager() constructed
main starts
Config() constructed
same object? 1, b.get("mode") = release
main ends
~Config()
```

**Explanation:**

- `Eager::inst` is a static member object → it is constructed during static initialization, **before** `main`.
- `Config::instance()` holds a function-local `static` → it is constructed on the **first call** (inside one of the threads), after "main starts".
- Since C++11 the initialization of a function-local static is thread-safe: if 4 threads race into `instance()`, exactly one constructs it and the others wait → printed **once**.
- Every call returns a reference to the same object → `&a == &b`; a change through `a` is visible through `b`.
- The private constructor and the deleted copy constructor/assignment make a second instance impossible. The static object is destroyed after `main` returns (`~Config()`).

**Exam relevance:** Singleton = one instance + a global access point. Typical questions: why the constructor is private, why copy is deleted, lazy vs eager initialization, thread safety (the old `if (!p) p = new T;` is a data race; double-checked locking needs atomics; the Meyers singleton is the simple answer). Drawbacks: hidden global state, harder unit testing.

### 02_factory — Factory, Factory Method and Abstract Factory

**Question:** What does ShapeFactory::create return for "SQUARE" and for "TRIANGLE"? Which button does each Dialog create? Which family of widgets does each GUI factory produce, and can a Windows button be mixed with a Linux checkbox?

**Expected output:**

```text
Simple factory:
  Circle::draw
  Square::draw
  TRIANGLE -> nullptr
Factory method:
  dialog shows <button>OK</button>
  dialog shows [ OK ]
Abstract factory:
  [ OK ] + [x] (Windows)
  ( OK ) (GTK) + (x) (GTK)
```

**Explanation:**

- Simple factory: the caller passes a string and gets a `unique_ptr<Shape>`; it never names `Circle` or `Square`. An unknown type returns `nullptr`.
- Factory Method: `Dialog::show()` is written once and calls the virtual `createButton()`; each subclass decides which concrete button to create.
- Abstract Factory: `WinFactory` only creates Windows widgets and `LinuxFactory` only GTK widgets, so a family is always consistent; `buildUi` works with any factory through the `GuiFactory` interface.
- Returning `unique_ptr` makes ownership explicit and avoids leaks.

**Exam relevance:** Factory hides `new` and the concrete class; Factory Method uses inheritance (a subclass decides); Abstract Factory uses composition and creates **families** of related objects. Adding a new product family to an abstract factory is easy (a new factory class); adding a new product kind is hard (every factory changes).

### 03_builder_prototype — Builder and Prototype

**Question:** Which items and total cost does each meal have? What does the fluent HttpRequest builder produce? After cloning the prototype and changing the clone's tags, does the original change? With the shallow clone, does it?

**Expected output:**

```text
Veg meal:
  - Veg burger 25
  - Coke 30
  total 55
Non-veg meal:
  - Chicken burger 50.5
  - Pepsi 35
  total 85.5
request: POST /api/runs, 1 header(s), body {}
  original: Q3 report [draft ]
  clone   : Q3 report [draft final ]
shallow copy: a.pages = 99, same pointer? 1
```

**Explanation:**

- The meal builder assembles a `Meal` step by step; the same construction steps produce different representations (veg / non-veg).
- The fluent builder returns `*this` from every setter, so calls chain; `build()` returns the finished object, and unset fields keep their defaults.
- `Report::clone()` uses the copy constructor; `std::vector` copies its elements, so the clone is a **deep** copy and the original keeps only "draft".
- `ShallowDoc` holds a raw pointer; the default copy copies the address, so both objects point to the same int → changing it through `b` changes `a`.

**Exam relevance:** Builder: many optional parameters / complex construction (avoids "telescoping constructors"). Prototype: create by cloning, via a virtual `clone()` (a "virtual constructor"); be careful with deep vs shallow copies.

### 04_adapter_bridge — Adapter and Bridge

**Question:** Which files can AudioPlayer play directly, which through the adapter, and which not at all? What does each bridged shape print? How many classes would you need for 3 shapes × 3 renderers with plain inheritance, and with Bridge?

**Expected output:**

```text
Adapter:
  built-in mp3: beyond the horizon.mp3
  mp4 player: alone.mp4
  vlc player: far far away.vlc
  invalid media: avi
Bridge:
  red circle r=10 at (100,100)
  green circle r=10 at (100,100)
```

**Explanation:**

- mp3 is played by `AudioPlayer` itself; mp4 and vlc go through `MediaAdapter`, which implements the `MediaPlayer` interface and forwards to an `AdvancedMediaPlayer`; avi is not supported.
- Bridge: `Circle` (the abstraction) holds a `DrawAPI` (the implementation); the same circle code draws red or green depending on the object passed in.
- With inheritance, 3 shapes × 3 renderers = 9 classes; with Bridge, 3 + 3 = 6, and both sides grow independently.

**Exam relevance:** Adapter makes **existing** incompatible interfaces work together (after the fact); Bridge is designed **up front** to separate abstraction and implementation. Object adapter (composition) vs class adapter (multiple inheritance).

### 05_composite_decorator — Composite and Decorator

**Question:** What is the total salary of the CEO's tree, and of the marketing head's subtree? In what order are the employees printed? What does each decorated coffee cost and print, and does the order of decorators change the description?

**Expected output:**

```text
Composite:
  John (CEO, 30000)
    Robert (Head Sales, 20000)
      Richard (Sales, 10000)
      Rob (Sales, 10000)
    Michel (Head Marketing, 20000)
      Laura (Marketing, 10000)
      Bob (Marketing, 10000)
  total salary: CEO tree = 110000, marketing subtree = 40000
Decorator:
  Espresso = 2
  Espresso + milk + sugar = 2.75
  Espresso + sugar + milk + milk = 3.25
```

**Explanation:**

- Composite: `totalSalary()` and `print()` are the same calls on a leaf and on a group; recursion walks the tree depth-first: 30000 + 2 × 20000 + 4 × 10000 = 110000; marketing = 20000 + 2 × 10000 = 40000.
- Decorator: each add-on wraps another `Beverage` and adds to its result. `Sugar(Milk(Espresso))` = 2 + 0.5 + 0.25; `Milk(Milk(Sugar(Espresso)))` = 2 + 0.25 + 0.5 + 0.5. The description follows the wrapping order (innermost first).

**Exam relevance:** Composite = part–whole hierarchies (file systems, GUI widgets, org charts), uniform treatment of leaves and composites. Decorator = adding responsibilities at run time without a subclass explosion (C++ streams, middleware); it keeps the same interface, unlike an adapter.

### 06_facade_proxy_flyweight — Facade, Proxy and Flyweight

**Question:** How many lines does computer.start() print? When is the image actually loaded from disk, and how many times for two display() calls? How many Circle objects does the flyweight factory create for 10 circles in 3 colors?

**Expected output:**

```text
Facade:
  cpu: freeze
  disk: read boot sector
  memory: load bootloader at 31744
  cpu: jump to 31744
  cpu: execute
Proxy:
  proxy created, nothing loaded yet
  loading photo.jpg from disk
  displaying photo.jpg
  displaying photo.jpg
Flyweight:
  10 circles drawn, Circle objects created = 3
```

**Explanation:**

- The facade's single `start()` drives three subsystems (5 steps); the client never touches `Cpu`, `Memory` or `Disk`.
- The proxy creates the `RealImage` only on the first `display()` (lazy loading) and reuses it → loaded once, displayed twice. 0x7c00 is printed in decimal (31744).
- The flyweight factory caches one `Circle` per color → only 3 objects for 10 draws; the position is passed in on each call.

**Exam relevance:** Facade = a simplified entry point to a subsystem. Proxy kinds: virtual (lazy), protection (access control), remote, caching, smart pointer. Flyweight = sharing intrinsic state to save memory (characters in a text editor, particles, game tiles).

### 07_observer — Observer (publish/subscribe), with detaching and weak_ptr observers

**Question:** Which observers print after setState(15)? After detaching the octal observer and calling setState(10)? The log observer is held through a weak_ptr: what happens after it is destroyed?

**Expected output:**

```text
setState(15):
  binary: 00001111
  octal:  17
  hex:    F
  log:    state is now 15
setState(10):
  binary: 00001010
  hex:    A
  log:    state is now 10
setState(3):
  binary: 00000011
  hex:    3
  (a weak observer is gone, removing it)
```

**Explanation:**

- Every attached observer is notified, in attach order.
- After `detach(&oct_)` the octal observer is no longer called.
- The subject stores the log observer as a `weak_ptr`; after `log.reset()` the object is gone, `lock()` returns an empty pointer and the subject removes the entry instead of calling a dead object.

**Exam relevance:** Observer = one-to-many notification, loose coupling (event systems, MVC views, signals/slots). Pitfalls: dangling observers (detach in the destructor, or use `weak_ptr`), modifying the list while iterating, notification order, observers triggering more updates.

### 08_strategy_state_template — Strategy, State and Template Method

**Question:** What does the context compute with each strategy? Which states does the turnstile go through for coin, push, push, coin, coin? In what order does Game::play() call the steps for Cricket and for Football?

**Expected output:**

```text
Strategy:
  add(10, 5) = 15
  subtract(10, 5) = 5
  multiply(10, 5) = 50
State:
  start: Locked
  coin: unlock
  push: pass through, lock again
  push while locked: blocked
  coin: unlock
  coin while unlocked: refund
  end: Unlocked
Template method:
  cricket initialized
  cricket started
  cricket finished
  football initialized
  football started
  game over
```

**Explanation:**

- Strategy: the calculator does not change; only the injected operation object does.
- State: each state class handles events and replaces the turnstile's state itself: Locked —coin→ Unlocked —push→ Locked —push (blocked)→ Locked —coin→ Unlocked —coin (refund)→ Unlocked.
- Template method: `play()` is non-virtual and fixes the order initialize → start → end; Football does not override `end()`, so the default hook prints "game over".

**Exam relevance:** Strategy vs State: the same structure (delegation to an interface), but with Strategy the **client** chooses the algorithm, while with State the **states** switch themselves. Template Method uses inheritance; Strategy uses composition.

### 09_command_memento_chain — Command (with undo), Memento and Chain of Responsibility

**Question:** What is the stock quantity after the queued orders run, and after undoing the last one? Which saved state does the editor return to after restore(0)? For each log level, which loggers in the chain write the message?

**Expected output:**

```text
Command:
  2 orders queued, qty still 10
  bought 5, qty = 15
  sold 3, qty = 12
  undo:
  bought 3, qty = 15
  final qty = 15
Memento:
  current:  State #1, #2, #3
  restore(0): State #1
  restore(1): State #1, #2
Chain of responsibility:
  INFO message:
    console: this is information
  DEBUG message:
    file:    this is debug info
    console: this is debug info
  ERROR message:
    error:   this is an error
    file:    this is an error
    console: this is an error
```

**Explanation:**

- Command: orders are objects; queuing them does nothing until `placeAll()`. 10 + 5 − 3 = 12; undoing the sell buys 3 back → 15.
- Memento: `Editor::Memento` is opaque (private data, friend `Editor`); the caretaker (the vector) stores snapshots without looking inside; restoring brings back exactly the saved text.
- Chain: every logger in the chain gets the message and writes it if the message level ≥ its own level: INFO → console only; DEBUG → file and console; ERROR → all three.

**Exam relevance:** Command enables undo/redo, queues, macros and logging of requests (GUI actions, job queues). Memento = undo/snapshots without breaking encapsulation. Chain of Responsibility = request handlers (logging levels, event bubbling, middleware); the sender does not know which handler acts.

### 10_iterator_visitor_mediator — Iterator, Visitor and Mediator

**Question:** What does the range-for over NameRepository print, and the explicit hasNext()/next() loop? What do the two visitors print, and what is the total price? Who receives each chat message, and does a user receive their own message?

**Expected output:**

```text
Iterator:
  range-for: Robert John Julie Lora
  hasNext/next: Robert John Julie Lora
Visitor:
  displaying keyboard (104 keys)
  displaying mouse
  displaying monitor (27")
  total price = 245
Mediator:
  John <- Robert: Hi John!
  Julie <- Robert: Hi John!
  Robert <- John: Hello Robert!
  Julie <- John: Hello Robert!
```

**Explanation:**

- Both iteration styles walk the same names in order; the repository never exposes its `vector`.
- Visitor: `p->accept(v)` calls `v.visit(*this)` with the static type of `*this`, which picks the right overload (double dispatch). The price visitor accumulates 30 + 15 + 200 = 245.
- Mediator: users only talk to the chat room, which forwards a message to everyone except the sender.

**Exam relevance:** Iterator = traversal without exposing the representation (STL iterators). Visitor = add operations without changing the element classes, but adding a new element type changes every visitor. Mediator reduces many-to-many dependencies (air-traffic control, chat rooms, GUI dialogs).

### 11_interpreter_filter_null — Interpreter, Filter (Criteria) and Null Object

**Question:** Which inputs do the rules "is male" (Robert OR John) and "is married woman" (Julie AND married) accept? Which persons does each criteria filter keep? What does the program print for the customers "Rob" and "Bob", and does it ever check for nullptr?

**Expected output:**

```text
Interpreter:
  John is male? 1
  Julie is male? 0
  "Married Julie" is a married woman? 1
  "Julie" is a married woman? 0
Filter:
  males: Robert John Mike Bobby
  females: Laura Diana
  single males: Robert Mike Bobby
  single or female: Robert Diana Mike Bobby Laura
Null object:
  Rob -> Rob
  Bob -> Not available in the customer database
  Julie -> Julie
  Laura -> Not available in the customer database
```

**Explanation:**

- Interpreter: the rules are expression trees; `Or` and `And` evaluate their children recursively; the terminals check whether the word occurs in the context.
- Filter: each criteria object returns a filtered list; `AndCriteria` applies one criteria to the result of the other; `OrCriteria` merges both results without duplicates (single people first, then the females not yet included).
- Null object: `findCustomer` never returns `nullptr`; unknown names get a `NullCustomer` whose `name()` returns a neutral message, so the loop needs no `if`.

**Exam relevance:** Interpreter suits small grammars (rule engines, SQL/regex parsers, expression evaluators). Filter/Criteria = composable selection rules. Null Object removes null checks and the crashes caused by forgetting them.

### 12_j2ee_patterns — Enterprise (J2EE) patterns in C++: MVC, DAO + Transfer Object, Business Delegate + Service Locator

**Question:** What does the view print before and after the controller changes the model? What does the DAO hold after update and delete? How many times is each service looked up "from the registry" when the delegate is called three times?

**Expected output:**

```text
MVC:
  view: Student Robert (roll no 10)
  view: Student John (roll no 10)
DAO + transfer object:
  all: [0 Robert] [1 John]
  dao: student 0 updated
  dao: student 1 deleted
  all: [0 Michael]
Business delegate + service locator:
  locator: looking up EJB in the registry
  processing in the EJB service
  locator: looking up JMS in the registry
  processing in the JMS service
  locator: EJB from cache
  processing in the EJB service
```

**Explanation:**

- MVC: the controller changes the model and asks the view to render it; the view only displays.
- DAO: callers use `getAll/update/remove` and never see how rows are stored. The `Student` copy returned by `getAll()` is a transfer object: changing it does nothing until it is passed back to `update()`.
- Service locator: the first lookup of each service goes to the "registry" (expensive) and is cached; the third call (EJB again) comes from the cache. The business delegate hides all of this from the presentation code.

**Exam relevance:** MVC = separation of concerns in UI code. DAO = a persistence abstraction (swap databases without touching business code). Transfer Object (DTO) = a plain data carrier between layers. Service Locator = a central, caching registry of services (an alternative to dependency injection).
