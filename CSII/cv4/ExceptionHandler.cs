namespace cv4
{
    public class ExceptionHandler
    {
        private readonly IMyLogger logger;

        public ExceptionHandler(IMyLogger logger)
        {
            this.logger = logger;
        }

        public async Task Handle(Exception ex)
        {
            string message = ex.Message + "\n" + ex.StackTrace;

            await logger.Log(message);

            //await File.AppendAllTextAsync("log.txt", ex.Message + "\n" + ex.StackTrace + "\n\n");
        }
    }
}
