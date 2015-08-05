% PROJET DE CONCEPTION - CENTRALESUPELEC - CAMPUS GIF
% 2014/2015 Séq. 8
% Alexandre LOEBLEIN HEINEN | Clyvian RIBEIRO BORGES
% CAMERA THERMIQUE
% Ce script récupere les données de la caméra thermique et fair leur filtrage
% Le résultat est affiché sur la GUI
%% initilisation des dispositifs et variables
clc;
com = 'COM3';               % identifiant de la porte serial COM
set(handles.dispositif,'String',strcat('Arduino Uno - ',{' '},com));
s = serial(com);            % initilisation de la communication serial
fopen(s);                   %
pause(1);                   % 1s pour que ça soit bien prêt

%% initialisation des variables
cols = 4;                   % donnees du capteur 16 x 4
rows = 16;                  % l'orientation c'est 'wide' = vertical
image = zeros(rows,cols);   % initilise la matrice de l'image 16x4

% la matrice globale (l'image composée par plusieures sous-images)
numIm = 3;                  % matrice carrée numIm x numIm
globalImage = cell(numIm);
for i = 1:numIm
    for j = 1:numIm
        globalImage{i,j} = image; 
    end
end

%% BOUCLE DE LECTURE
% on l'execute pendant que le pushbutton2 est activé
while strcmp(get(handles.pushbutton2,'Enable'),'on') 
    i = 1;      % compteur des lignes
    
    while(i <= rows)
        j = 1;                              % compteur des colonnes
        while(j <= cols)                    % cette boucle assure qu'aucune affichage ne sera faite si l'on n'a pas reçu une flag (<= -300)
            while(s.BytesAvailable == 0)    % on attent que il y ait des données disponibles
                
            end
            
            pixel = str2double(fscanf(s));  % lecture de la porte seriale
            image(rows-i+1,j) = pixel;
            j = j + 1;              % on continue les boucle
        end
        i = i + 1;
    end
    
    axes(handles.axes1);
    imagesc(image);                      % dessin des donnés de la matrice (chaque fois)
    caxis([15, 40]);                     % les limites de la température
    colorbar;                            % on ajoute une barre pour montrer l'echelle de température
    title('Image originale');

    n = round(str2num(get(handles.edit1,'String')));
    r = round(str2num(get(handles.edit2,'String')));
    imageProc = imageProcess(image, n, r);
    axes(handles.axes2);
    imagesc(imageProc);
    caxis([15, 40]);                     % les limites de la température
    colorbar;                            % on ajoute une barre pour montrer l'echelle de température
    title('Image filtrée');
    %%end
    pause(.1);
end

saveas(gcf, strcat('camera_', datestr(now,'dd.mm.yy_HH.MM.SS'),'.png'));               % on sauvagarde la derniere image
%% fin
fclose(s);
delete(s);
clear s;