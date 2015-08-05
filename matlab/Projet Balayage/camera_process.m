% PROJET DE CONCEPTION - CENTRALESUPELEC - CAMPUS GIF
% 2014/2015 S�q. 8
% Alexandre LOEBLEIN HEINEN | Clyvian RIBEIRO BORGES
% CAMERA THERMIQUE
% Ce script r�cupere va sauvegarder toutes les images affich�es !
%% initilisation des dispositifs et variables
clc;
com = 'COM3';               % identifiant de la porte serial COM
set(handles.dispositif,'String',strcat('Arduino Uno - ',{' '},com));
s = serial(com);            % initilisation de la communication serial
fopen(s);                   %
pause(1);                   % 1s pour que �a soit bien pr�t

%% initialisation des variables
cols = 4;                   % donnees du capteur 16 x 4
rows = 16;                  % l'orientation c'est 'wide' = vertical
image = zeros(rows,cols);   % initilise la matrice de l'image 16x4

% la matrice globale (l'image compos�e par plusieures sous-images)
numIm1 = 1;                  % matrice carr�e numIm x numIm
numIm2 = 4;
globalImage = cell(numIm1, numIm2);
for i = 1:numIm1
    for j = 1:numIm2
        globalImage{i,j} = image; 
    end
end

%% BOUCLE DE LECTURE
% on l'execute pendant que le pushbutton2 est activ�
while strcmp(get(handles.pushbutton2,'Enable'),'on') 
    i = 1;      % compteur des lignes
    flag = 0;   % 0 - avant la lecture;
                % 1 - lecture de l'image
    
    while(i <= rows)
        j = 1;                              % compteur des colonnes
        while(j <= cols)                    % cette boucle assure qu'aucune affichage ne sera faite si l'on n'a pas re�u une flag (<= -300)
            while(s.BytesAvailable == 0)    % on attent que il y ait des donn�es disponibles
                
            end
            
            pixel = str2double(fscanf(s));  % lecture de la porte seriale
            
            if (pixel <= -300 && flag == 0) % la lecture ne sera initialis�e qu'apr�s la flag
               ind = (abs(pixel+300));
               c = mod(ind,10);             % determine la colonnes actuelle
               r = floor(ind/10);           % determine la ligne actuelle
               set(handles.temp, 'String', strcat(num2str(r),'x',num2str(c)));
               flag = 1;                    % on arrive � la lecture des temp�ratures
            else if (flag == 1)             % une fois que la lecture a commenc�
                    image(rows-i+1,j) = pixel;
                    j = j + 1;              % on continue les boucle
                end
            end
        end
        i = i + 1;
    end
    
    globalImage{numIm1-r, numIm2-c} = image; % on compose l'image globale
    
    if (r == (numIm1-1) && c == (numIm2 - 1)) % l'affichage n'est faite qu'apr�s arriver � la derniere sous-image
        matrice = cell2mat(globalImage);         % transformation d'une cellule dans une matrice 
        axes(handles.axes1);
        imagesc(matrice);                      % dessin des donn�s de la matrice (chaque fois)
        caxis([15, 40]);                     % les limites de la temp�rature
        colorbar;                            % on ajoute une barre pour montrer l'echelle de temp�rature
        title('Image originale');
        
        n = round(str2num(get(handles.edit1,'String')));
        rad = round(str2num(get(handles.edit2,'String')));
        imageProc = imageProcess(matrice, n, rad);
        axes(handles.axes2);
        imagesc(imageProc);
        caxis([15, 40]);                     % les limites de la temp�rature
        colorbar;                            % on ajoute une barre pour montrer l'echelle de temp�rature
        title('Image filtr�e');
    end
    pause(.1);
    %saveas(gcf, strcat('camera_', datestr(now,'dd.mm.yy_HH.MM.SS'),''));               % on sauvagarde la derniere image
end


%% fin
fclose(s);
delete(s);
clear s;