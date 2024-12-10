if vim.fn.has("mac") ~= 0 then
elseif vim.fn.has("win64") ~= 0 then
	vim.opt.makeprg="build"
end
