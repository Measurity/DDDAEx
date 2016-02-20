namespace DDDAEx.Net
{
    public interface IPacket
    {
        /// <summary>
        /// Identification number to exchange over IP with minimal impact.
        /// </summary>
        short Id { get; set; }
    }
}