using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Net.Http.Json;
using System.Text;
using System.Threading.Tasks;

namespace ServerCore
{
    public class ApiInterface
    {
        private HttpClient _httpClient { get; set; }

        public ApiInterface(string uri, string apiKey)
        {
            _httpClient = new HttpClient { BaseAddress = new Uri(uri) };
            _httpClient.DefaultRequestHeaders.Add("X-API-KEY", apiKey);
        }

        private void AddBearerToken(HttpRequestMessage request, string token)
        {
            request.Headers.Authorization = new System.Net.Http.Headers.AuthenticationHeaderValue("Bearer", token);
        }

        public async Task<(HttpResponseMessage httpResponse, TResponse? response)> SendPostRequestAsync<TResponse>(string endpoint, Dictionary<string, string> formData, string? jwtToken = null)
        {
            var httpRequest = new HttpRequestMessage(HttpMethod.Post, endpoint)
            {
                Content = new FormUrlEncodedContent(formData)
            };

            if (!string.IsNullOrEmpty(jwtToken))
            {
                AddBearerToken(httpRequest, jwtToken);
            }

            var httpResponse = await _httpClient.SendAsync(httpRequest);
            return (httpResponse, await httpResponse.Content.ReadFromJsonAsync<TResponse>());
        }

        public async Task<(HttpResponseMessage httpResponse, TResponse? response)> SendGetRequestAsync<TResponse>(string endpoint, string? jwtToken = null)
        {
            var httpRequest = new HttpRequestMessage(HttpMethod.Get, endpoint);

            if (!string.IsNullOrEmpty(jwtToken))
            {
                AddBearerToken(httpRequest, jwtToken);
            }

            var httpResponse = await _httpClient.SendAsync(httpRequest);
            return (httpResponse, await httpResponse.Content.ReadFromJsonAsync<TResponse>());
        }
    }
}
