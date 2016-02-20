using ProtoBuf;

namespace DDDAEx.Net.Packets
{
    [ProtoContract(SkipConstructor = true)]
    public class ChatPacket : IPacket
    {
        public ChatPacket(string message)
        {
            Message = message;
        }

        [ProtoMember(1)]
        public short Id { get; set; }
        [ProtoMember(2)]
        public string Message { get; set; }
    }
}