  
function main (target)
	local host = os.host()
	local subhost = os.subhost()

	local system
	if (host ~= subhost) then
		system = host .. "/" .. subhost
	else
		system = host
	end

	local branch = "unknown-branch"
	local commit = "unknown-commit"
	local timestamp = ""
	try
	{
		function ()
			import("detect.tools.find_git")
			local git = find_git()
			if (git) then
				branch = os.iorunv(git, {"rev-parse", "--abbrev-ref", "HEAD"}):trim()
				commit = os.iorunv(git, {"rev-parse", "--short", "HEAD"}):trim()
				timestamp = os.iorunv(git, {"log", "-1", "--date=short", "--pretty=format:%ci"}):trim()
			else
				error("git not found")
			end
		end,

		catch
		{
			function (err)
				print(string.format("Failed to retrieve git data: %s", err))
			end
		}
    }

    return branch, commit, timestamp
end