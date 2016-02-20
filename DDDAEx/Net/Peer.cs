using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using ProtoBuf;

namespace DDDAEx.Net
{
    public class Peer
    {
        public const int DefaultPort = 8080;
        public const int DefaultBackLog = 10;

        protected Peer(int port)
        {
            CancellationTokenSource = new CancellationTokenSource();
            ConnectionList = new List<TcpClient>();
            Server = new TcpListener(IPAddress.Any, port);
            Client = new TcpClient();
        }

        public TcpListener Server { get; }
        public TcpClient Client { get; set; }
        public CancellationTokenSource CancellationTokenSource { get; set; }

        protected List<TcpClient> ConnectionList { get; }

        public IEnumerable<TcpClient> Connections => ConnectionList;

        public static Peer Create(int port = DefaultPort)
        {
            var peer = new Peer(port);
            peer.AcceptClientsAsync();
            return peer;
        }

        public void Connect(string host, int port)
        {
            Client.Connect(host, port);
        }

        protected async Task AcceptClientsAsync()
        {
            Server.Start(DefaultBackLog);
            while (!CancellationTokenSource.IsCancellationRequested)
            {
                await StartReceiveAsync(await Server.AcceptTcpClientAsync().ConfigureAwait(false)).ConfigureAwait(false);
            }
        }

        protected async Task StartReceiveAsync(TcpClient client)
        {
            ConnectionList.Add(client);
            while (!CancellationTokenSource.IsCancellationRequested)
            {
                using (client)
                {
                    var buffer = new byte[1024 * 4];
                    var stream = client.GetStream();

                    var timeoutTask = Task.Delay(TimeSpan.FromSeconds(15));
                    var amountReadTask = stream.ReadAsync(buffer, 0, buffer.Length, CancellationTokenSource.Token);
                    var completedTask = await Task.WhenAny(timeoutTask, amountReadTask).ConfigureAwait(false);

                    if (completedTask == timeoutTask)
                    {
                        var msg = Encoding.ASCII.GetBytes("Client timed out");
                        await stream.WriteAsync(msg, 0, msg.Length);
                        break;
                    }

                    var amountRead = amountReadTask.Result;
                    if (amountRead == 0) break; // End of stream.
                    await stream.WriteAsync(buffer, 0, amountRead, CancellationTokenSource.Token)
                        .ConfigureAwait(false);
                    
                    // TODO: Deserialize incoming bytes to packet based on first 2 bytes (short id).
                }
            }
            ConnectionList.Remove(client);
        }

        /// <summary>
        ///     Sends a <see cref="IPacket" /> to all connected clients with TCP.
        /// </summary>
        /// <param name="packet"><see cref="IPacket" /> to send to another Socket.</param>
        public void TransmitAll(IPacket packet)
        {
            Serializer.Serialize(Client.GetStream(), packet);
        }
    }
}