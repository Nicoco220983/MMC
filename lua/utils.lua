local lib = {}

local BinaryFormat = package.cpath:match("%p[\\|/]?%p(%a+)")
if BinaryFormat == "dll" then os_name = "Windows"
elseif BinaryFormat == "so" then os_name = "Linux"
elseif BinaryFormat == "dylib" then os_name = "MacOS"
else error("Cannot determine OS")
end

function file_exists(path)
    local file = io.open(path, "r")
    if file == nil then return false end
    io.close(file)
    return true
end
lib.file_exists = file_exists

function is_dir(path)
    return file_exists(path .. '/')
end
lib.is_dir = is_dir

function bin_exists(ctx, name)
    local cmd
    if os_name == "Windows" then
        cmd = "where " .. name
    else 
        cmd = "which " .. name
    end
    p = io.popen(cmd)
    res = p:read("*a"):len() > 0
    p:close()
    return res
end
lib.bin_exists = bin_exists

function list_dir(path)
    local cmd
    if os_name == "Windows" then
        cmd = 'dir "'..path..'" /b /ad'
    else 
        cmd = 'ls -a "'..path..'"'
    end
    local p = io.popen(cmd)
    local i, res = 0, {}
    for fname in p:lines() do
        if fname ~= "." and fname ~= ".." then
            i = i + 1
            res[i] = fname
        end
    end
    p:close()
    return res
end
lib.list_dir = list_dir

local dir_sep
if os_name == "Windows" then
    dir_sep = '\\'
else 
    dir_sep = '/'
end

function join(path1, path2)
    if path2 == nil then return path1
    else return path1 .. dir_sep .. path2 end
end
lib.join = join

return lib