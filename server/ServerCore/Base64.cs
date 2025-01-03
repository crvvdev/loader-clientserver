using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerCore
{
    public static class Base64
    {
        public static int DecodedSize(this string base64) =>
        (base64.Length * 3 + 3) / 4 - (base64.Length > 0 && base64[^1] == '=' ? base64.Length > 1 && base64[^2] == '=' ? 2 : 1 : 0);

        public static Task<bool> IsBase64Async(this string base64) =>
            Task.FromResult(IsBase64(base64));

        public static bool IsBase64(this string base64)
        {
            var bufferSize = DecodedSize(base64);
            Span<byte> buffer = stackalloc byte[bufferSize]; // avoid array heap allocation
            return Convert.TryFromBase64String(base64, buffer, out _);
        }

        public static string ToBase64(this string str)
        {
            return Base64Encode(str);
        }

        public static string FromBase64(this string encoded)
        {
            return Base64Decode(encoded);
        }

        public static string Base64Decode(string encoded)
        {
            if (!encoded.IsBase64())
            {
                throw new InvalidDataException("The input string is not a valid Base64 string.");
            }

            byte[] decoded = Convert.FromBase64String(encoded);
            return Encoding.UTF8.GetString(decoded);
        }

        public static string Base64Encode(byte[] data)
        {
            return Convert.ToBase64String(data);
        }

        public static string Base64Encode(string data)
        {
            byte[] byteData = Encoding.UTF8.GetBytes(data);
            return Base64Encode(byteData);
        }
    }
}
