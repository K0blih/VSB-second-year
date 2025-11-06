using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Messages
{
    public class MessageSerializer<T> : MessageSerializerBase where T : IMessage, new()
    {
        public IEncryption Encryption { get; set; }
        public MessageSerializer(IEncryption encryption)
        {
            Encryption = encryption;
        }

        public string Serialize(T obj)
        {
            return $"{obj.GetMark()}{Encryption.GetMark()}{Encryption.Encrypt(obj.Serialize())}";
        }
        
        public T Deserialize(string inputString)
        {
            T message = new T();
            if (inputString[0] != message.GetMark())
            {
                throw new InvalidMessageException();
            }
            if (inputString[1] != Encryption.GetMark())
            {
                throw new InvalidMessageException();
            }

            string encryptedString = inputString.Substring(2);
            string decryptedString = Encryption.Decrypt(encryptedString);

            message.Deserialize(decryptedString);

            return message;
        }
    }
}
