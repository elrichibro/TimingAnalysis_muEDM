#include "Visualizer.hpp"


// VISUALIZATION
#include <TLegend.h>


int PlotTOT(TCanvas* c_all, ROOT::RDF::RNode node, const std::vector<int>& channels, int bins, double min, double max, int run_id) { 
    if (!c_all) return -1;
    
    c_all->cd();
    c_all->Clear();

    const std::vector<int> colors = {
        kBlue + 1, kRed + 1, kGreen + 2, kMagenta + 1, 
        kOrange + 7, kCyan + 2, kViolet - 1, kTeal + 2
    };

    std::vector<ROOT::RDF::RResultPtr<TH1D>> hist_ptrs;
    hist_ptrs.reserve(channels.size());

    for (int ch : channels) {
        std::string col_name = "ToT_Ch" + std::to_string(ch);
        std::string hist_name = "h_ToT_Ch" + std::to_string(ch) + "_run" + std::to_string(run_id);
        std::string hist_title = "ToT;ToT [ns];Events";

        hist_ptrs.push_back(node.Histo1D({hist_name.c_str(), hist_title.c_str(), bins, min, max}, col_name));
    }

    double max_y = 0.0;
    long long total_entries = 0;

    for (size_t i = 0; i < hist_ptrs.size(); i++) {
        double current_max = hist_ptrs[i]->GetMaximum();
        total_entries += hist_ptrs[i]->GetEntries();
        if (current_max > max_y) {
            max_y = current_max;
        }
    }

    if (total_entries == 0) {
        printf("WARNING: no data in [%.2f, %.2f]!\n", min, max);
    }

    auto legend = new TLegend(0.72, 0.68, 0.88, 0.88);
    legend->SetBorderSize(1);
    legend->SetFillColor(0);
    legend->SetTextSize(0.03);
    std::string header_title = "Channels (Run " + std::to_string(run_id) + ")";
    legend->SetHeader(header_title.c_str(), "C");

    for (size_t i = 0; i < hist_ptrs.size(); i++) {
        auto h = hist_ptrs[i].GetPtr();
        int color = colors[i % colors.size()];

        std::string clone_name = std::string(h->GetName()) + "_copy";
        TH1D* h_clone = (TH1D*) h->Clone(clone_name.c_str());
        h_clone->SetDirectory(0);

        h_clone->SetLineColor(color);
        h_clone->SetLineWidth(2);
        h_clone->SetStats(0);

        if (max_y > 0) {
            h_clone->SetMaximum(max_y * 1.2);
        }

        const char* draw_option = (i == 0) ? "HIST" : "HIST SAME";
        h_clone->Draw(draw_option);

        std::string leg_label = "Channel " + std::to_string(channels[i]);
        legend->AddEntry(h_clone, leg_label.c_str(), "l");
    }

    legend->Draw();

    c_all->Modified();
    c_all->Update();

    return 0;
}



int PlotTOA_MEAN(TCanvas* c_all, ROOT::RDF::RNode node, const std::vector<int>& channels, int bins, double min, double max, int run_id) {
    if (!c_all) {
        return -1;
    }

    c_all->cd();
    c_all->Clear();

    const std::vector<int> colors = {
        kBlue + 1, kRed + 1, kGreen + 2, kMagenta + 1, 
        kOrange + 7, kCyan + 2, kViolet - 1, kTeal + 2
    };

    std::vector<ROOT::RDF::RResultPtr<TH1D>> hist_ptrs;
    hist_ptrs.reserve(channels.size());

    std::string hist_title = "ToA;ToA [ns];Events";

    for (int ch : channels) {
        std::string col_name = "FirstToA_Ch" + std::to_string(ch) + "_Lead";
        std::string hist_name = "h_ToA_Ch" + std::to_string(ch) + "_run" + std::to_string(run_id);    

        hist_ptrs.push_back(node.Histo1D({hist_name.c_str(), hist_title.c_str(), bins, min, max}, col_name));
    }

    std::string hist1 = "h_ToA_Mean_run" + std::to_string(run_id);
    hist_ptrs.push_back(node.Histo1D({hist1.c_str(), hist_title.c_str(), bins, min, max}, "TOA_Mean"));

    std::string hist2 = "h_ToA_Mean_run" + std::to_string(run_id);
    hist_ptrs.push_back(node.Histo1D({hist2.c_str(), hist_title.c_str(), bins, min, max}, "Event_ToA_0_0"));

    double max_y = 0.0;
    long long total_entries = 0;

    for (size_t i = 0; i < hist_ptrs.size(); i++) {
        double current_max = hist_ptrs[i]->GetMaximum();
        total_entries += hist_ptrs[i]->GetEntries();
        if (current_max > max_y) {
            max_y = current_max;
        }
    }

    if (total_entries == 0) {
        printf("WARNING: no data in [%.2f, %.2f]!\n", min, max);
    }

    auto legend = new TLegend(0.72, 0.68, 0.88, 0.88);
    legend->SetBorderSize(1);
    legend->SetFillColor(0);
    legend->SetTextSize(0.03);
    std::string header_title = "Channels (Run " + std::to_string(run_id) + ")";
    legend->SetHeader(header_title.c_str(), "C");

    for (size_t i = 0; i < hist_ptrs.size(); i++) {
        auto h = hist_ptrs[i].GetPtr();
        int color = colors[i % colors.size()];

        std::string clone_name = std::string(h->GetName()) + "_copy";
        TH1D* h_clone = (TH1D*) h->Clone(clone_name.c_str());
        h_clone->SetDirectory(0);

        h_clone->SetLineColor(color);
        h_clone->SetLineWidth(2);
        h_clone->SetStats(0);

        if (max_y > 0) {
            h_clone->SetMaximum(max_y * 1.2);
        }

        const char* draw_option = (i == 0) ? "HIST" : "HIST SAME";
        h_clone->Draw(draw_option);

        std::string leg_label = "Channel " + std::to_string(channels[i]);
        legend->AddEntry(h_clone, leg_label.c_str(), "l"); 
    }

    legend->Draw();

    c_all->Modified();
    c_all->Update();

    return 0;
}



int Plot_DeltaFirstToA(TCanvas* c_all, ROOT::RDF::RNode node, const first_ToA& f_ToA, const plot_config& plot) {
    if (!c_all) return -1;
    
    c_all->cd();
    c_all->Clear();

    auto hist_deltaToA = node.Histo1D({"hist_deltaToA", plot.title.c_str(), plot.bins, plot.min, plot.max}, f_ToA.Delta);

    hist_deltaToA->DrawClone(); 

    c_all->Update();
    
    return 0;
}

int Plot_Hits(TCanvas* c_all, ROOT::RDF::RNode node, const config_struct& cfg, const std::string& column) {
    if (!c_all) return -1;
    
    c_all->cd();
    c_all->Clear();

    auto plot = cfg.canvas.plot_hits;
    auto hist_hits = node.Histo1D({"hist_hits", plot.title.c_str(), plot.bins, plot.min, plot.max}, column);

    hist_hits->DrawClone(); 

    c_all->Update();
    
    return 0;
}