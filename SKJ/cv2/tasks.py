def is_palindrome(data):
    """
    Returns True if `data` is a palindrome and False otherwise.
    Hint:
        slicing is your friend, use it
    Example:
        is_palindrome('aba') == True
        is_palindrome('abc') == False
    """
    i = 0
    for c in data[::-1]:
        if c != data[i]:
            return False
        i += 1

    return True


def count_successive(string):
    """
    Go through the string and for each character, count how many times it appears in succession.
    Store the character and the count in a tuple and return a list of such tuples.

    Example:
          count_successive("aaabbcccc") == [("a", 3), ("b", 2), ("c", 4)]
          count_successive("aba") == [("a", 1), ("b", 1), ("a", 1)]
    """
    l = []
    if string == "":
        return []
    
    prev = string[0]
    i = 0
    for c in string:
        if c != prev:
            l.append((prev, i))
            prev = c
            i = 1
        else:
            i += 1
    l.append((prev, i))

    return l


def create_appender(default_value):
    """
    Create an empty list. Return a function which will append its only parameter to this list
    when called and then return a copy of the modified list.
    If no parameter is passed, it will add `default_value` to the list.

    Example:
        appender = create_appender(5)
        appender(1) # [1]
        appender(2) # [1, 2]
        appender() # [1, 2, 5]

        appender2 = create_appender(0)
        appender2(2) # [2]
    """
    l = []
    def appender(item = default_value):
        l.append(item)

        return l.copy()

    return appender


def fibonacci_closure():
    """
    Return a closure (function) that will generate elements of the Fibonacci sequence (starting
    from 1) when called repeatedly.
    Example:
        g = fibonacci_closure()
        g() # 1
        g() # 1
        g() # 2
        g() # 3
        ...
    """
    prev = 0
    curr = 1

    def fibonacci():
        nonlocal prev
        nonlocal curr
        value = curr
        prev, curr = curr, prev + curr
        return value

    return fibonacci

import re

def word_extractor(sentence):
    """
    Return a generator that will iterate through individual words from the input sentence.
    Words are separated by the following separators: space (' '), dot ('.'), exclamation mark ('!')
    and question mark ('?'). Skip empty words and separators.

    If you encounter the word "stop", ignore the word and stop the generator.

    Example:
        sentence = "Hello world. How are you doing today? I'm doing fine!"
        for word in word_extractor(sentence):
            print(i)
        # "Hello", "world", "How", "are", "you", "doing", "today", ...

        sentence = "Hello world stop this is not extracted anymore."
        for word in word_extractor(sentence):
            print(i)
        # "Hello", "world"
    """
    words = re.split(r"[ .!?]", sentence)

    for word in words:
        if word == "":
            continue
        if word == "stop":
            return
        yield word


def tree_walker(tree, order):
    """
    Write a generator that traverses `tree` in the given `order` ('inorder', 'preorder' or 'postorder').
    You should know this from 'Algoritmy II'.
    The tree is represented with nested tuples (left subtree, value, right subtree).
    If there is no subtree, it will be marked as None.
    Example:
        tree = (((None, 8, None), 3, (None, 4, None)), 5, (None, 1, None))
            5
           / \
          3   1
         / \
        8   4
        list(tree_walker(tree, 'inorder')) == [8, 3, 4, 5, 1]
        list(tree_walker(tree, 'preorder')) == [5, 3, 8, 4, 1]
        list(tree_walker(tree, 'postorder')) == [8, 4, 3, 1, 5]
    """
    if tree is None:
        return

    left, value, right = tree

    if order == "inorder":
        yield from tree_walker(left, order)
        yield value
        yield from tree_walker(right, order)
    elif order == "preorder":
        yield value
        yield from tree_walker(left, order)
        yield from tree_walker(right, order)
    elif order == "postorder":
        yield from tree_walker(left, order)
        yield from tree_walker(right, order)
        yield value
