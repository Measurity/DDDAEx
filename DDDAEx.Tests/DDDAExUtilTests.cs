using DDDAEx;
using System;
using System.IO;
using NUnit.Framework;
using Assert = NUnit.Framework.Assert;

namespace DDDAEx.Tests
{
    [TestFixture]
    public class DDDAExUtilTests
    {
        [Test]
        public void InstallPathTest()
        {
            Assert.NotNull(DDDAExUtil.InstallPath);
        }

        [Test]
        public void ExePathTest()
        {
            Assert.IsTrue(File.Exists(DDDAExUtil.ExePath));
        }
    }
}
