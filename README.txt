APPROACH:

  Main_Window___________________________________________________________
  ColorHistogram_widget_mainLayout_____________________________________|
  | viewLayout               |  histLayout                             |
  |    ____________          |    ___________________                  |
  |   |           |          |    |                 |<----showSlice---||
  |   |   Image   |          |  ->| histViewer class|    mouse        ||
  |   |   Viewer  |          |  | |_________________|---|Moved        ||
  |   |   Class   |          |  --____statusbar______ <-|Signal       ||
  |   |           |    mouse |    |-colValSlider-----|                ||
  |   |___________|---|Moved |       [ QComboBox ] -> regenSlices-----||
  |   __statusbar__ <-|Signal|    |-thresholderSlider| -> regenSlices- |
  |__________________________|_________________________________________|
  |____________________________________________________________________|

CONSIDERATIONS/REFLECTIONS:
- Reconsider overall hierarchy with histViewer. While trying to emulate ImageViewer, I realized that histViewer signal would emit only position of mouse to histStatusBar and I would need to extract R,G,B for statusBar from colorHistgoram directly. I really don't like that I am splitting info between a signal + the parent class.
- In future, find way of keeping the original QLabel pixmap of the histogram while also tracking mouse position like in ImageViewer
- Avoid ugly switch cases (perhaps better way of handling of ColorHistogram::mouseMoveHist comes with first bullet point)
- Consider generating color value slices individually rather than regenerating all slices (more efficient in long run but overhead for colorValSlider?)
- I realized in class on the last lab that I don't have to manually do bitshifts for indexing hist... I could just have easily done qRgb(...) && 0xFF...F or smthn like that
- I don't think I would have been able to implement live statusbar for histViewer without having ImageViewer from lecture as reference... Consider practicing/going over this!
