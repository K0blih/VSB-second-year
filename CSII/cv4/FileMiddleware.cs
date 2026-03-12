namespace cv4
{
    public class FileMiddleware
    {
        private readonly RequestDelegate next;

        public FileMiddleware(RequestDelegate next)
        {
            this.next = next;
        }

        public async Task Invoke(HttpContext ctx)
        {
            string dir = "wwwroot";

            string path = ctx.Request.Path.Value.TrimStart(new char[] {'/', '.'}); 

            string filePath = Path.Combine(dir, path);

            if (File.Exists(filePath))
            {
                ctx.Response.Headers.ContentType = "image/jpeg";
                await ctx.Response.SendFileAsync(filePath);
            }
            else
            {
                await next(ctx);
            }
        }
    }
}
