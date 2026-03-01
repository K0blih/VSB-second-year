/**
 * Implement a RAII Logger class that will print a line to a specified output stream in its
 * constructor and destructor.
 *
 * The class will receive an output stream and some text in its constructor. It will print
 * <text> [start]
 * in its constructor and
 * <text> [end]
 * in its destructor to the specified output stream (std::ostream&).
 *
 *
 * Each logger will print its output with some indent (number of space characters printed before the
 * actual content of the line).
 * When you create a logger with a constructor, it will have indent of `0` space characters.
 *
 * The Logger will also have a method called `sublogger`, which will receive text to be printed.
 * It will return a new logger that will use the same output stream, it will have its indent increased
 * by `2` and it will print the provided `text`.
 *
 * Make sure that the class cannot be copied or copy assigned.
 *
 * Example:
 * ```
 * {
 *    Logger logger(std::cout, "foo"); // prints "foo [start]"
 * } // prints "foo [end]"
 *
 * {
 *    Logger l1(std::cout, "l1"); // prints "l1 [start]"
 *    {
 *        Logger l2 = l1.sublogger(); // prints "  l2 [start]"
 *    } // prints "  l2 [end]"
 * } // prints "l1 [end]"
 * ```
 */

class Logger {};
