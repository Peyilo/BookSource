print("== Complex QuickJS Test ==");

// ============================
// 1. Class + inheritance
// ============================
class Animal {
    constructor(name) {
        this.name = name;
    }
    speak() {
        return `[Animal:${this.name}] generic sound`;
    }
}

class Dog extends Animal {
    constructor(name, age) {
        super(name);
        this.age = age;
    }
    speak() {
        return `[Dog:${this.name}] woof! age=${this.age}`;
    }
}

let dog = new Dog("Buddy", 3);
print("Class test:", dog.speak());

// ============================
// 2. Promises + async/await
// ============================
function delay(ms) {
    return new Promise(resolve => {
        // 如果你在 C/C++ 中实现了 setTimeout，可以用 setTimeout
        // 否则用 hack: resolve immediately
        resolve(ms);
    });
}

async function asyncTest() {
    let v1 = await delay(10);
    let v2 = await delay(20);
    return v1 + v2;
}

asyncTest().then(r => print("Async/Await:", r));

// ============================
// 3. Map / Set / spread operator
// ============================
let m = new Map();
m.set("a", 1);
m.set("b", 2);

let set = new Set([1, 2, 3]);
print("Map size:", m.size);
print("Set has 2:", set.has(2));

let arr = [...set, ...m.values()];
print("Spread:", arr.join(","));

// ============================
// 4. Generator
// ============================
function* gen() {
    for (let i = 0; i < 3; i++) {
        yield i * 2;
    }
}
let g = gen();
print("Generator:", g.next().value, g.next().value, g.next().value);

// ============================
// 5. TypedArray
// ============================
let buf = new ArrayBuffer(8);
let u32 = new Uint32Array(buf);
u32[0] = 123;
u32[1] = 456;
print("TypedArray:", u32[0], u32[1]);

// ============================
// 6. Proxy
// ============================
let target = { x: 1 };
let proxy = new Proxy(target, {
    get(obj, prop) {
        if (prop === "magic") return 999;
        return obj[prop];
    },
    set(obj, prop, val) {
        obj[prop] = val * 2;
        return true;
    }
});

print("Proxy get:", proxy.magic);
proxy.x = 10;
print("Proxy set:", proxy.x);

// ============================
// 7. Error & try/catch
// ============================
try {
    throw new Error("Test error");
} catch (e) {
    print("Error caught:", e.message);
}

// ============================
// 8. RegExp
// ============================
let re = /h(.*)o/;
let result = "hello".match(re);
print("RegExp match:", result[1]);

// ============================
// 9. JSON
// ============================
let jsonObject = { a: 1, b: [1, 2, 3], nested: { x: true } };
let jsonStr = JSON.stringify(jsonObject);
print("JSON:", jsonStr);
print("Parsed:", JSON.parse(jsonStr).nested.x);

// ============================
// 10. Date API
// ============================
let now = new Date();
print("Date:", now.toISOString());

// ============================
// 11. BigInt
// ============================
let big = 123456789012345678901234567890n;
print("BigInt:", big + 10n);

// ============================
// 12. Function.bind
// ============================
function testBind(x, y) { return this.k + x + y; }
let objBind = { k: 10 };
let f = testBind.bind(objBind, 5);
print("Bind:", f(20));

// ============================
// 13. Deep clone test
// ============================
let obj = { a: 1, b: { c: 2 }};
let clone = JSON.parse(JSON.stringify(obj));
clone.b.c = 99;
print("DeepClone original:", obj.b.c);
print("DeepClone clone:", clone.b.c);

// ============================
// 14. Optional chaining
// ============================
let opt = { foo: { bar: 42 } };
print("Optional:", opt.foo?.bar, opt.foo?.baz?.xxx);

// ============================
// 15. Nullish coalescing
// ============================
let nn1 = null ?? 123;
let nn2 = 0 ?? 456;
print("Nullish:", nn1, nn2);

// ============================
// 16. Template string
// ============================
let a1 = 10, a2 = 20;
print(`Template: ${a1 + a2}`);

// ============================
// 17. Unicode / UTF-8 test
// ============================
print("Unicode:", "中文测试 🚀");

// ============================
// 18. Object assign
// ============================
let o1 = { x: 1 };
let o2 = { y: 2 };
let merged = Object.assign({}, o1, o2);
print("Object.assign:", merged.x, merged.y);

// ============================
// 19. Destructure
// ============================
let { x, y } = { x: 10, y: 20 };
print("Destructure:", x, y);

// ============================
// 20. Recursive function
// ============================
function fib(n) {
    if (n < 2) return n;
    return fib(n - 1) + fib(n - 2);
}
print("Fib(10):", fib(10));

print("== Test End ==");
