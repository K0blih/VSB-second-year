using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Messages
{
    public class MessageQueue<T>
    {
        private class Node
        {
            public T Value { get; set; }
            public Node Next { get; set; }

            public Node(T value)
            {
                Value = value;
                Next = null;
            }
        }
        private Node head;
        private Node tail;
        public bool IsEmpty
        {
            get
            {
                return head == null;
            }
        }

        public void Enqueue(T item)
        {
            Node newNode = new Node(item);

            if (IsEmpty)
            {
                head = tail = newNode;
            }
            else
            {
                tail.Next = newNode;
                tail = newNode;
            }
        }

        public T Dequeue()
        {
            if (IsEmpty)
            {
                throw new EmptyQueueException();
            }

            T value = head.Value;
            head = head.Next;

            if (head == null)
            {
                tail = null;
            }

            return value;
        }
    }
}
