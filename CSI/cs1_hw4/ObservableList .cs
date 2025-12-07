using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Encodings.Web;
using System.Text.Json;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace cs1_hw4
{
    internal class Utf8StringWriter : StringWriter
    {
        public override Encoding Encoding
        {
            get { return Encoding.UTF8; }
        }
    }
    internal class ObservableList<T> : IEnumerable<T>
    {
        private List<T> items = new List<T>();

        //public delegate void ItemChangedHandler(T item);
        //public event ItemChangedHandler? OnAdd;
        //public event ItemChangedHandler? OnRemove;
        public event Action<T>? OnAdd;
        public event Action<T>? OnRemove;

        public T this[int index]
        {
            get { return items[index]; }
            set { items[index] = value; }
        }

        public void Add(T item)
        {
            items.Add(item);
            OnAdd?.Invoke(item);
        }

        public void Remove(T item)
        {
            items.Remove(item);
            OnRemove?.Invoke(item);
        }

        public string SerializeToJson()
        {
            return JsonSerializer.Serialize(items, new JsonSerializerOptions()
            {
                PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
                Encoder = JavaScriptEncoder.UnsafeRelaxedJsonEscaping,
                WriteIndented = true
            });
        }

        public string SerializeToXml()
        {
            XmlSerializer serializer = new XmlSerializer(typeof(List<T>));
            using StringWriter sw = new Utf8StringWriter();
            serializer.Serialize(sw, items);
            
            return sw.ToString();
        }

        public static ObservableList<T> DeserializeFromJson(string json)
        {
            List<T> items = JsonSerializer.Deserialize<List<T>>(json, new JsonSerializerOptions
            {
                PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
                Encoder = JavaScriptEncoder.UnsafeRelaxedJsonEscaping
            })!;

            ObservableList<T> list = new ObservableList<T>();
            foreach (T item in items)
            {
                list.Add(item);
            }
            return list;
        }

        public static ObservableList<T> DeserializeFromXml(string xml)
        {
            XmlSerializer serializer = new XmlSerializer(typeof(List<T>));
            using StringReader sr = new StringReader(xml);
            List<T> items = (List<T>)serializer.Deserialize(sr)!;

            ObservableList<T> list = new ObservableList<T>();
            foreach (T item in items)
            {
                list.Add(item);
            }

            return list;
        }

        public IEnumerator<T> GetEnumerator()
        {
            int start = 0;
            int end = items.Count - 1;

            while (start <= end)
            {
                if (start == end)
                {
                    yield return items[start];

                }
                else
                {
                    yield return items[start];
                    yield return items[end];
                }
                start++;
                end--;
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        //public delegate bool FilterPredicate<T>(T item);
        public IEnumerable<T> Filter(Predicate<T> predicate)
        {
            foreach (T item in this)
            {
                if (predicate(item))
                {
                    yield return item;
                }
            }
        }

        public List<T> OrderBy(IComparer<T> comparer)
        {
            List<T> sorted = new List<T>(items);
            sorted.Sort(comparer);

            return sorted;
        }
    }
}
