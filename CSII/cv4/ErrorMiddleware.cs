namespace cv4
{
    public class ErrorMiddleware
    {
        private readonly RequestDelegate next;

        public ErrorMiddleware(RequestDelegate next)
        {
            this.next = next;
        }

        public async Task Invoke(HttpContext context, ExceptionHandler handler)
        {
            try
            {
                await next(context);
            }
            catch (Exception ex)
            {
                context.Response.StatusCode = 500;
                context.Response.Headers.ContentType = "text/plain; charset=utf-8";
                await context.Response.WriteAsync("Došlo k chybě!");

                await handler.Handle(ex);

                //await File.AppendAllTextAsync("log.txt", ex.Message + "\n" + ex.StackTrace + "\n\n");
            }
        }
    }
}
