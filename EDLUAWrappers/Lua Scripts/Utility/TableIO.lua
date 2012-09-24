-- Table IO functions
-- Author:  Chelsey Squanda
-- Purpose:  Have robust functions to read and write tables

do
	-- Returns a "Lua" portable version of the string
	-- Parameters
		-- _szString = string
	local function ExportString(_szString)
		_szString = string.format( "%q",_szString );
		-- Replace all instances
		_szString = string.gsub( _szString,"\\\n","\\n" );
		_szString = string.gsub( _szString,"\r","\\r" );
		_szString = string.gsub( _szString,string.char(26),
			"\"..string.char(26)..\"" );
		return _szString;
	end

	-- Saves out a table to the specified lua file
	-- Parameters
		-- _tTable = table
		-- _szFilename = string
	function SaveTable(  _tTable,_szFilename )
		local cStart, cEnd = "   ", "\n";
		local ioFile, eError;
	
		-- Attempt to open the lua file
		ioFile, eError = io.open(_szFilename, "w");
		-- Check to make sure the file opened
		if eError then
			-- Print the failed file and return
			GetError("TableIO.lua: " .._szFileName .. " has failed to open");
			return;
		end
	
		-- initiate variables for save procedure
		local tTables,tLookup = { _tTable },{ [_tTable] = 1 };
		ioFile:write("return {" ..cEnd);

		-- Loop through all tables in tTables
		for index, t in ipairs(tTables) do
			-- Write out the current table's index
			ioFile:write("-- Table: {" ..index .."}" ..cEnd);
			ioFile:write("{" ..cEnd);

			-- Loop through all the values in the current table
			local tHandled = {};
			for i, v in ipairs(t) do
				tHandled[i] = true;
				-- Escape functions and userdata
				if type(v) ~= "userdata" then
					-- Only handle value
					if type(v) == "table" then
						if not tLookup[v] then
							table.insert(tTables, v);
							tLookup[v] = #tTables;
						end

						-- Write out value
						ioFile:write(cStart .."{"..tLookup[v] .."}," ..cEnd);
					elseif type(v) == "function" then
						-- Write out function
						ioFile:write(cStart .."loadstring(" ..ExportString(string.dump( v )) ..")," ..cEnd);
					else
						-- Write out everything else
						local value = (type(v) == "string" and ExportString(v)) or tostring(v);
						ioFile:write(cStart ..value .."," ..cEnd);
					end
				end
			end

			-- Loop through all the indices in the current table
			for i, v in pairs(t) do
				-- Escape functions and userdata
				if (not tHandled[i]) and type(v) ~= "userdata" then
					-- Handle index
					if type( i ) == "table" then
						if not tLookup[i] then
							table.insert(tTables,i);
							tLookup[i] = #tTables;
						end

						-- Write out the index information
						ioFile:write(cStart .."[{" ..tLookup[i] .."}]=");
					else
						-- Write out everything else
						local index = (type(i) == "string" and "["..ExportString(i).."]") or string.format("[%d]",i);
						ioFile:write(cStart ..index .."=");
					end

					-- Handle value
					if type(v) == "table" then
						if not tLookup[v] then
							table.insert(tTables,v);
							tLookup[v] = #tTables;
						end

						-- Write out the value
						ioFile:write("{" ..tLookup[v] .."}," ..cEnd);
					elseif type(v) == "function" then
						-- Write out the function
						ioFile:write("loadstring(" ..ExportString(string.dump( v )) ..")," ..cEnd);
					else
						-- Write out everything else
						local value = (type(v) == "string" and ExportString(v)) or tostring(v);
						ioFile:write(value .."," ..cEnd);
					end
				end
			end

			ioFile:write("},"..cEnd);
		end

		ioFile:write("}");
	
		-- Close the file
		ioFile:close();
	end

	-- Loads in a table from the specified lua file
	-- Parameters
		-- _szFilename = string
	function LoadTable( _szFilename )
		-- Attempt to load the file
		tTables, eError = loadfile(_szFilename);
		-- Check to make sure the file loaded
		if eError then
			-- Print the failed file and return
			GetError("TableIO.lua: " .._szFileName .. " has failed to open");
			return;
		end
	
		-- Loop through and pull all information out of the file
		tTables = tTables();
		for index = 1, #tTables do
			local tToLinkV, tToLinkI = {}, {};
			for i, v in pairs(tTables[index]) do
				-- Pull value information
				if type(v) == "table" and tTables[v[1]] then
					table.insert(tToLinkV,{ i,tTables[v[1]] });
				end

				-- Pull index information
				if type(i) == "table" and tTables[i[1]] then
					table.insert(tToLinkI,{ i,tTables[i[1]] });
				end
			end

			-- Link values first due to possible changes of indices
			for _, v in ipairs(tToLinkV) do
				tTables[index][v[1]] = v[2];
			end

			-- Link indices
			for _, v in ipairs(tToLinkI) do
				tTables[index][v[2]], tTables[index][v[1]] = tTables[index][v[1]],
					nil;
			end
		end

		return tTables[1];
	end
-- close do
end