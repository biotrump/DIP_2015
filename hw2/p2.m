function varargout = p2(varargin)
% P2 MATLAB code for p2.fig
%      P2, by itself, creates a new P2 or raises the existing
%      singleton*.
%
%      H = P2 returns the handle to a new P2 or the handle to
%      the existing singleton*.
%
%      P2('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in P2.M with the given input arguments.
%
%      P2('Property','Value',...) creates a new P2 or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before p2_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to p2_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help p2

% Last Modified by GUIDE v2.5 08-Apr-2016 00:53:57

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @p2_OpeningFcn, ...
                   'gui_OutputFcn',  @p2_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before p2 is made visible.
function p2_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to p2 (see VARARGIN)

% Choose default command line output for p2
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes p2 wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = p2_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;
