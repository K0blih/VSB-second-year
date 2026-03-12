namespace cv4
{
    public class BrowserAuthMiddleware
    {
        private readonly RequestDelegate next;

        public BrowserAuthMiddleware(RequestDelegate next)
        {
            this.next = next;
        }

        public async Task Invoke(HttpContext context)
        {
            string ua = context.Request.Headers.UserAgent;
            if (ua.Contains("Chrome") && !ua.Contains("Edg"))
            //if (context.Request.Query.TryGetValue("apikey", out var key) && key == "abc")
            {
                await next(context);
            }
            else
            {
                context.Response.Headers.ContentType = "text/html; charset=utf-8";
                await context.Response.WriteAsync("Pouzij Chrome!");
            }
        }
    }
}
