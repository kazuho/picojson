using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;
using System.Threading;


namespace CSharpClient
{
    public class Client
    {
        [DataContract]
        public class Message
        {
            public enum MessageType { ProtocolVersion, Action };
            [DataMember]
            public MessageType CustomMessage { get; set; }
            [DataMember]
            public int Argument { get; set; }
            [DataMember]
            public string StringMessage { get; set; }
            public const int ProtocolVersion = 1;
        }


        private IPEndPoint serverEndpoint = new IPEndPoint(IPAddress.Loopback, 21307);
        private TcpClient tcpClient;
        public bool IsInitialized { get { return this.tcpClient != null ? this.tcpClient.Connected : false; } }
        public TimeSpan ReceiveWaitTime { get; set; }


        public Client()
        {
            tcpClient = new TcpClient();
            tcpClient.Connect(serverEndpoint);
            ReceiveWaitTime = TimeSpan.FromMilliseconds(300);
        }


        public void Send(Message message)
        {
            if (!tcpClient.Connected)
                return;
            
            using (var mStream = new MemoryStream())
                using (var streamReader = new StreamReader(mStream))
                {
                    var binWriter = new BinaryWriter(tcpClient.GetStream());
                    new DataContractJsonSerializer(typeof(Message)).WriteObject(mStream, message);
                    mStream.Seek(0, SeekOrigin.Begin);
                    binWriter.Write(Encoding.ASCII.GetBytes(streamReader.ReadToEnd()));
                }
        }

        public Message Receive()
        {
            var startReceiving = DateTime.Now;
            while (DateTime.Now - startReceiving < ReceiveWaitTime)
            {
                if (tcpClient.Available > 0)
                {
                    try
                    {
                        var binReader = new BinaryReader(tcpClient.GetStream());
                        var stringData = Encoding.ASCII.GetString(binReader.ReadBytes(tcpClient.Available));
                        var mStream = new MemoryStream(Encoding.UTF8.GetBytes(stringData));
                        return ((Message) new DataContractJsonSerializer(typeof(Message)).ReadObject(mStream));
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine("Unexpected excepiton occured  (Client.Receive): " + e.Message);
                    }
                }
            }

            Console.WriteLine("Unable to receive a message: waited too long or error occured  (Client.Receive)");
            return null;
        }
    }



    class Program
    {
        static void Main(string[] args)
        {
            Thread.Sleep(500); //Let the server start first
            try
            {
                Console.WriteLine("Program started. Trying to get connected...");
                Client client = new Client();
                if (client.IsInitialized)
                {
                    Console.WriteLine("Connected to server\n");
                    client.Send(new Client.Message() { CustomMessage = Client.Message.MessageType.ProtocolVersion, Argument = Client.Message.ProtocolVersion });
                    Console.WriteLine("Sent message with protocol version\n");
                    var message = client.Receive();
                    if (message != null)
                    {
                        Console.WriteLine("Got message from server");
                        Console.WriteLine("Message type: " + Enum.GetName(typeof(Client.Message.MessageType), message.Argument));
                        Console.WriteLine("Message arg:  {0}", message.Argument);
                        Console.WriteLine("Message string: {0}\n", message.StringMessage);
                    }
                }
                else
                {
                    Console.WriteLine("Unable to get connected\n");
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Unexpected exception occured  (Main): {0}\n", e.Message);
            }
            Console.WriteLine("Execution finished. Press any key");
            Console.ReadKey();
        }
    }
}
