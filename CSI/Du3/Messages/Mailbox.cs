using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Messages
{
    public class Mailbox
    {
        private readonly MessageQueue<string> queue;
        private readonly List<MessageSerializerBase> serializers;
        public Mailbox(MessageQueue<string> q)
        {
            queue = q;
            serializers = new List<MessageSerializerBase>();
        }

        public void SendMessage<T>(MessageSerializer<T> serializer, T message) where T : IMessage, new()
        {
            queue.Enqueue(serializer.Serialize(message));
        }

        public void RegisterSerializer(MessageSerializerBase serializer)
        {
            serializers.Add(serializer);
        }

        public bool TryReceive<T>(string serializedMessage, out T message) where T : IMessage, new()
        {
            message = default;

            foreach (MessageSerializerBase serializerBase in serializers)
            {
                if (serializerBase is MessageSerializer<T> serializer)
                {
                    try
                    {
                        message = serializer.Deserialize(serializedMessage);
                        return true;
                    }
                    catch (InvalidMessageException)
                    {
                    }
                }
            }

            return false;
        }
    }
}
