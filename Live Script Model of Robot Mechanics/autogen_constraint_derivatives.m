function [A_all,H_bw_x,H_bw_y,H_fw_y] = autogen_constraint_derivatives(r_bw)
%AUTOGEN_CONSTRAINT_DERIVATIVES
%    [A_ALL,H_BW_X,H_BW_Y,H_FW_Y] = AUTOGEN_CONSTRAINT_DERIVATIVES(R_BW)

%    This function was generated by the Symbolic Math Toolbox version 8.5.
%    04-May-2020 14:56:40

A_all = reshape([1.0,0.0,0.0,0.0,1.0,1.0,0.0,0.0,0.0,-r_bw,0.0,0.0,0.0,0.0,0.0],[3,5]);
if nargout > 1
    H_bw_x = reshape([0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0],[5,5]);
end
if nargout > 2
    H_bw_y = reshape([0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0],[5,5]);
end
if nargout > 3
    H_fw_y = reshape([0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0],[5,5]);
end
