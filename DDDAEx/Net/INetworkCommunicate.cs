using System.Collections.Generic;
using System.Net.Sockets;

namespace DDDAEx.Net
{
    public interface INetworkCommunicate
    {
        Socket Local { get; }
        IEnumerable<Socket> Connections { get; }
        bool IsRunning { get; }

        
    }
}