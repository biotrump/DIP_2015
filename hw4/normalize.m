%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%normalize the min to max range to 0.0-1.0
function output_image = normalize(input_image)
	MIN = min(min(input_image)); MAX = max(max(input_image));
	output_image = (input_image - MIN) / (MAX - MIN);
end
