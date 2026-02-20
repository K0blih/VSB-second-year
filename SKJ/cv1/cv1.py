for x in 'asdfghjk':
    print(x, end='')
print()

for x in range(0,10):
    print(x, end='')
print()

def add(a, b):
    return a + b

print(add(10, 20))

def fact(n):
    if n < 1:
        return 1
    return n * fact(n - 1)

print(fact(5))

def simple_redact(text, char):
    result = ''
    for c in text:
        if c == char:
            result += '!'
        else:
            result += c

    return result

print(simple_redact("cau", 'a'))

for x in zip([0,1,2,3,4], [5,6,7,8,9]):
    print(x)