local function lerp(a,b,t)
  return a + (b-a)*t
end

local function clamp(a,x,b)
  if x < a then
    return a
  end

  if x > b then
    return b
  end

  return x
end

local function cap3(v, m)
  if math.abs(v:x()) > m then
    return v / v:x() * m
  end
  if math.abs(v:y()) > m then
    return v / v:y() * m
  end
  if math.abs(v:z()) > m then
    return v / v:z() * m
  end
  return v
end

local function drawEffect(fx, tech, drawfn)
  local numPasses = fx:start(tech)
  for i=1,numPasses do
    fx:beginPass(i)
    drawfn(fx)
    fx:endPass()
  end
  fx:finish()
end

local function spairs(t, order)
  -- collect the keys
  local keys = {}
  for k in pairs(t) do keys[#keys+1] = k end

  -- if order function given, sort by it by passing the table and keys a, b,
  -- otherwise just sort the keys
  if order then
    table.sort(keys, function(a,b) return order(t[a], t[b]) end)
  else
    table.sort(keys)
  end

  -- return the iterator function
  local i = 0
  return function()
    i = i + 1
    if keys[i] then
        return keys[i], t[keys[i]]
    end
  end
end

local function dump(o)
  if type(o) == 'table' then
     local s = '{ '
     for k,v in pairs(o) do
        if type(k) ~= 'number' then k = '"'..k..'"' end
        s = s .. '['..k..'] = ' .. dump(v) .. ','
     end
     return s .. '} '
  else
     return tostring(o)
  end
end

local function withTexture(tex, fn)
  BindTexture(0, tex)
  fn()
  BindTexture(0)
end

local function merge(a, b)
  if type(a) == 'table' and type(b) == 'table' then
    for k,v in pairs(b) do if type(v)=='table' and type(a[k] or false)=='table' then merge(a[k],v) else a[k]=v end end
  end
  return a
end

local function serializeTable(val, name, skipnewlines, depth, skipfuncs)
  skipnewlines = skipnewlines or false
  depth = depth or 0

  local tmp = string.rep(" ", depth)
  if name then
    if not string.match(name, '^[a-zA-z_][a-zA-Z0-9_]*$') then
      name = string.gsub(name, "'", "\\'")
      name = "['".. name .. "']"
    end
    tmp = tmp .. name .. " = "
   end

  if type(val) == "table" then
    tmp = tmp .. "{" .. (not skipnewlines and "\n" or "")

    for k, v in pairs(val) do
      tmp =  tmp .. serializeTable(v, k, skipnewlines, depth + 1) .. "," .. (not skipnewlines and "\n" or "")
    end

    tmp = tmp .. string.rep(" ", depth) .. "}"
  elseif type(val) == "number" then
    tmp = tmp .. tostring(val)
  elseif type(val) == "string" then
    tmp = tmp .. string.format("%q", val)
  elseif type(val) == "boolean" then
    tmp = tmp .. (val and "true" or "false")
  elseif not skipfuncs and type(val) == "function" then
    tmp = tmp .. string.format("%q", "fn:"..tostring(string.dump(val, true)))
  else
      tmp = tmp .. "\"[inserializeable datatype:" .. type(val) .. "]\""
  end

  return tmp
end

local function encode(table, skipfuncs)
  skipfuncs = skipfuncs or false
	return "return"..serializeTable(table, nil, nil, nil, skipfuncs)
end

local function decode(str)
  local f = load(str)()
  local fndump = "fn:"

  for k, v in pairs(f) do
    if type(v) == "string" and v:sub(1, #fndump) == fndump then
      f[k] = load(v:sub(#fndump+1, #v))
    end
  end
	return f
end

local function deepcopy(orig)
  local origType = type(orig)
  local copy
  if origType == 'table' then
    copy = {}
    for origKey, origValue in next, orig, nil do
        copy[deepcopy(origKey)] = deepcopy(origValue)
    end
    setmetatable(copy, deepcopy(getmetatable(orig)))
  else
    copy = orig
  end
  return copy
end

local function slice(tbl, first, last, step)
  local sliced = {}
  for i = first or 1, last or #tbl, step or 1 do
    sliced[#sliced+1] = tbl[i]
  end
  return sliced
end

local helpers = {
  lerp = lerp,
  clamp = clamp,
  cap3 = cap3,
  spairs = spairs,
  dump = dump,
  merge = merge,
  encode = encode,
  decode = decode,
  serializeTable = serializeTable,
  deepcopy = deepcopy,
  slice = slice,

  drawEffect = drawEffect,
  withTexture = withTexture,
  withEffect = drawEffect,
}

helpers.global = function()
  _G = merge(_G, helpers)
  return helpers
end

return helpers
