def reverse(text):
    yield text[::-1]

for n in reverse("Hello"):
    print(n, end='')
    
print()

def create_counter():
    count = 1
    def counter():
        nonlocal count
        print(count)
        count += 1
    return counter

c1 = create_counter()
c1()
c1()

print()

c2 = create_counter()
c2()
c2()