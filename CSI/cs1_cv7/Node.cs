using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv7
{
    internal class Node<T, U> : IKeyValuePair<T, U>
    {
        public T Key { get; set; }
        public U Value { get; set; }
        public Node<T, U> Left { get; set; }
        public Node<T, U> Right { get; set; }

        public Node(T key, U value)
        {
            Key = key;
            Value = value;
        }
    }
}
