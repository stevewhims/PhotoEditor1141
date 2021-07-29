#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "Photo.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Search;
using namespace Windows::Storage::Streams;
using namespace Microsoft::UI::Composition;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Data;
using namespace Microsoft::UI::Xaml::Hosting;
using namespace Microsoft::UI::Xaml::Media::Animation;
using namespace Microsoft::UI::Xaml::Media::Imaging;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PhotoEditor::implementation
{
    MainWindow::MainWindow() : m_photos(winrt::single_threaded_observable_vector<IInspectable>())
    {
        InitializeComponent();
    }

    // Loads collection of Photos from users Pictures library.
    IAsyncAction MainWindow::OnActivated(IInspectable const& sender, WindowActivatedEventArgs const& args)
    {
        // Load photos if they haven't previously been loaded.
        if (Photos().Size() == 0)
        {
            // File type filter.
            QueryOptions options{};
            options.FolderDepth(FolderDepth::Deep);
            options.FileTypeFilter().Append(L".jpg");
            options.FileTypeFilter().Append(L".png");
            options.FileTypeFilter().Append(L".gif");

            // Get the Pictures library.
            StorageFolder picturesFolder = KnownFolders::PicturesLibrary();
            auto result = picturesFolder.CreateFileQueryWithOptions(options);
            auto imageFiles = co_await result.GetFilesAsync();

            // Populate Photos collection.
            for (auto&& file : imageFiles)
            {
                // Only files on the local computer are supported. 
                // Files on OneDrive or a network location are excluded.
                if (file.Provider().Id() == L"computer")
                {
                    auto image = co_await LoadImageInfoAsync(file);
                    Photos().Append(image);
                }
            }
        }
    }

    IAsyncAction MainWindow::OnContainerContentChanging(ListViewBase const& sender, ContainerContentChangingEventArgs const& args)
    {
        auto elementVisual = ElementCompositionPreview::GetElementVisual(args.ItemContainer());
        auto image = args.ItemContainer().ContentTemplateRoot().as<Image>();

        if (args.InRecycleQueue())
        {
            image.Source(nullptr);
        }

        if (args.Phase() == 0)
        {
            args.RegisterUpdateCallback([&](auto sender, auto args)
            {
                OnContainerContentChanging(sender, args);
            });

            args.Handled(true);
        }

        if (args.Phase() == 1)
        {
            auto item = unbox_value<PhotoEditor::Photo>(args.Item());
            Photo* impleType = get_self<Photo>(item);

            try
            {
                auto thumbnail = co_await impleType->GetImageThumbnailAsync();
                image.Source(thumbnail);
            }
            catch (winrt::hresult_error ex)
            {
                // File could be corrupt, or it might have an image file
                // extension, but not really be an image file.
                BitmapImage bitmapImage{};
                Uri uri{ image.BaseUri().AbsoluteUri(), L"Images/StoreLogo.png" };
                bitmapImage.UriSource(uri);
                image.Source(bitmapImage);
            }
        }
        co_return;
    }

    // Creates a Photo from Storage file for adding to Photo collection.
    IAsyncOperation<PhotoEditor::Photo> MainWindow::LoadImageInfoAsync(StorageFile const& file)
    {
        //auto properties = co_await file.Properties().GetImagePropertiesAsync();
        auto info = winrt::make<Photo>(file);
        co_return info;
    }
}
