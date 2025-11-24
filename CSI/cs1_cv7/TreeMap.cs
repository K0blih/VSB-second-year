using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv7
{
    internal class TreeMap<TKey, TValue> where TKey : IComparable<TKey>
    {
        private Node<TKey, TValue> root;

        private TValue Get(TKey key, Node<TKey, TValue> current)
        {
            if (key.CompareTo(current.Key) == 0)
            {
                return current.Value;
            }
            if (key.CompareTo(current.Key) < 0)
            {
                return Get(key, current.Left);
            }
            return Get(key, current.Right);
        }
        private void Set(TKey key, TValue value, Node<TKey, TValue> current)
        {
            if (current.Key.CompareTo(key) < 0)
            {
                if (current.Right == null)
                {
                    current.Right = new Node<TKey, TValue>(key, value);
                }
                else
                {
                    Set(key, value, current.Right);
                }
            }
            else if (current.Key.CompareTo(key) > 0)
            {
                if (current.Right == null)
                {
                    current.Left = new Node<TKey, TValue>(key, value);
                }
                else
                {
                    Set(key, value, current.Left);
                }
            }
            else
            {
                current.Value = value;
            }
        }

        public TValue this[TKey key]
        {
            get
            {
                return Get(key, root);
            }
            set
            {
                if (root == null)
                {
                    root = new Node<TKey, TValue>(key, value);
                    return;
                }
                Set(key, value, root);
            }
        }

        public int Count
        {
            get
            {
                return GetCount(root);
            }
        }

        private int GetCount(Node<TKey, TValue> current)
        {
            if (current == null)
            {
                return 0;
            }
            return 1 + GetCount(current.Left) + GetCount(current.Right);
        }

        private void ToArray(Node<TKey, TValue>[] arr, ref int idx, Node<TKey, TValue> current)
        {
            if (current == null) { return; }
            arr[idx++] = current;
            ToArray(arr, ref idx, current.Left);
            ToArray(arr, ref idx, current.Right);
        }

        public Node<TKey, TValue>[] ToArray()
        {
            Node<TKey, TValue>[] result = new Node<TKey, TValue>[Count];

            int idx = 0;
            ToArray(result, ref idx, root);

            return result;
        }
    }
}
