-- Strict struct definition
-- Author:  Chelsey Squanda
-- Purpose:  Put the strict struct definition in its own file to avoid bloating
	-- from class declaration.

-- Defining the class constructor
local mtClass = {
	-- Instances can be created by calling the struct object
	__call = function(_tSelf, _tInstance)
		-- We can pass it a table or nothing
		local CObject = _tInstance or {};
		local tFields = _tSelf._fields;
		
		-- Attempt to set a non-existant field in constructor?
		-- Loop through all fields and check
		for i, v in pairs(CObject) do
			-- If field does not exist...
			if not tFields[i] then
				-- Throw an error
				_tSelf._error_nf(nil, i);
			end
		end

		-- Fill any default values if not supplied
		for i, v in pairs(tFields) do
			-- If there is nothing in the field...
			if not CObject[i] then
				-- Fill it in
				CObject[i] = v;
			end
		end

		-- Set the CObject's metatable
		setmetatable(CObject, _tSelf._mt);

		-- Return CObject
		return CObject;
	end
};

-- Creating a new class triggered by CClass.STRUCTNAME
Class = setmetatable({}, {
	__index = function(_tTable, _szName)
		-- We create a new struct object with a name
		local CNewClass = { _name = _szName };
		-- Put tStruct in the enclosing context
		_G[_szName] = CNewClass;
		
		-- Set the not-found error
		CNewClass._error_nf = function(_tTable, _szKey)
			-- Incorrect field remains nil
			-- GetError(lua_State* _state)
				-- Function takes a string
			GetError("StrictStruct.lua:  Field '" .._szKey .."' is not in " 
				..CNewClass._name);
		end

		-- Reading or writing an undefined field of this struct is an error
		CNewClass._mt = { _name = CNewClass._name;
						__index = CNewClass._error_nf;
						__newindex = CNewClass._error_nf; }

		-- The struct has a constructor
		setmetatable(CNewClass, mtClass);

		-- Return a function that sets tStruct's fields
		return function(_tFields)
			CNewClass._fields = _tFields;
		end
	end
});

Class.CSprite { m_szFilename = "Default";
				m_fPosX = 0; m_fPosY = 0;
				m_fWidth = 0; m_fHeight = 0;
				m_bHoriFlip = false; m_bVertiFlip = false; m_fRotation = 0;
				m_fAlpha = 1.0; m_fRed = 1.0; m_fGreen = 1.0; m_fBlue = 1.0;
				m_nLayer = 1; }