local Context = {} -- the table representing the class, which will double as the metatable for the instances
Context.__index = Context

function Context.new()
    local self = setmetatable({}, Context)
	self.inputPath = nil
	self.outputPath = nil
	self.outputMode = "COPY"
	self.compressionlevel = "M"
	self.imgMinLength = nil
	self.videoBitrate = nil
	self.videoCrf = nil
	self.imgCompressor = nil
	self.videoCompressor = nil
	self.force = false
	self.logLevel = "INFO"
    self.currentPath = nil
    return self
end

return Context