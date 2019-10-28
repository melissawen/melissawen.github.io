desc "compile and run the site"
task :default do
  pids = [
    spawn("jekyll server -w"),
    spawn("scss --watch assets:assets"),
  ]
 
  trap "INT" do
    Process.kill "INT", *pids
    exit 1
  end
 
  loop do
    sleep 1
  end
end
